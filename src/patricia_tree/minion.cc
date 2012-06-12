#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include "minion.hh"
#include "thread_pool.hh"

Minion::Minion(ThreadPool* pool, int num)
  : pool_ (pool)
{
  word_ = NULL;
  cmpTable_ = NULL;
  key_ = NULL;
  num_ = num;
}

Minion::~Minion()
{
  deleteTable();
}



void
Minion::tableDisplay(std::ostream& out, size_t keyLen)
{
  size_t i;
  size_t j;
  pool_->logLock();
  for (i = 0; i < cmpTableSize_; i++)
  {
    if (i == 0)
      out << "      ";
    else
      out << " " << key_[i - 1] << " ";
    if (i == keyLen)
      out << " ";
    else
      out << "|";
  }
  out << std::endl;
  out << "  -";
  for (i = 0; i < cmpTableSize_; i++)
    out << "----";
  out << std::endl;
  for (i = 0; i < cmpTableSize_; i++)
  {
    if (i == 0)
      out << "  ";
    else
      if (i < wordLen_ + 1)
	out << word_[i - 1] << " ";
      else
	out << "  ";
    for (j = 0; j < keyLen + 1 && j < cmpTableSize_; j++)
    {
      out << "|";
      if (cmpTable_[i][j] < 10)
	out << " ";
      out << (int)cmpTable_[i][j]
	  << " ";
    }
    if (j < cmpTableSize_)
    {
      out << "  ";
      for (; j < cmpTableSize_; j++)
      {
	out << (int)cmpTable_[i][j]
	    << " | ";
      }
    }
    out << std::endl;
  }
  out << "  -";
  for (i = 0; i < cmpTableSize_; i++)
    out << "----";
  out << std::endl;
  pool_->logUnlock();
}


bool
Minion::log(std::string msg)
{
  return true;
  pool_->logLock();
  for (int i = 0; i < num_; i++)
  {
    std::cout << "\t\t\t\t";
  }
  std::cout << "Thread " << num_ << ": "
	    << msg << std::endl;
  pool_->logUnlock();
  return true;
}


void
Minion::deleteTable()
{
  if (cmpTable_ != NULL)
  {
    for (size_t i = 0; i < cmpTableSize_; i++)
      delete[] cmpTable_[i];
    delete[] cmpTable_;
  }

  if (key_ != NULL)
    delete[] key_;
}



void
Minion::configure(const char* word,
		  unsigned int maxDistance,
		  const char* treeData,
		  std::list<SearchResult>& collector)
{
  word_ = word;
  maxDistance_ = maxDistance;
  collector_ = &collector;
  treeData_ = treeData;

  // Redim the table for distance calculation
  wordLen_ = strlen(word);
  deleteTable();
  cmpTableSize_ = wordLen_ + maxDistance_ + 1;

  size_t keyBufferSize = wordLen_ + maxDistance_;
  key_ = new char[keyBufferSize];
  for (size_t i = 0; i < keyBufferSize; i++)
    key_[i] = ' ';


  cmpTable_ = new unsigned char*[cmpTableSize_];
  assert(cmpTable_ != NULL);
  for (size_t i = 0; i < cmpTableSize_; i++)
  {
    cmpTable_[i] = new unsigned char[cmpTableSize_];
    assert(cmpTable_[i] != NULL);
    cmpTable_[0][i] = i;
    cmpTable_[i][0] = i;
    for (size_t j = 1; j < cmpTableSize_; j++)
      if (i != 0)
	cmpTable_[i][j] = 0;
  }
}


bool
Minion::getATask()
{
  log("getatask");
  pool_->todoListLock();
  if (!pool_->todoListIsNotEmpty())
  {
    // Ils nous volent notre travail !
    //std::cerr << "\t\t\tempty :(" << std::endl;
    pool_->todoListUnlock();
    log("leave");
    return false;
  }
  log("go");
  // Cool du boulot !
  nodeFetchTask* task = pool_->todoListPopTask();
  pool_->todoListUnlock();



  if (maxDistance_ == 0)
  {
    browseNode0(task->first, task->second.size());
  }
  else
  {
    reInitKey(task->second);
    //std::cout << task->second << std::endl;
    log("browse");
    //log("take a task");
    browseNode(task->first, task->second.size());
  }

  delete task;
  log("return");
  return true;
}



void
Minion::run()
{
  log("Let's Go !");
  // Infinite loop
  do
  {
    // Wake up
    log("wait to be configured");
    pool_->configureWaitMutex();
    if (pool_->wannaQuit())
    {
      log("pool wanna break (1)");
      break;
    }
    // Is there still something to do ?
    log("work");
    pool_->affectNbIdleThreads(-1);
    while(getATask())
    {
    }
    log("leave while");
    if (pool_->wannaQuit())
    {
      log("pool wanna break (2)");
      break;
    }
    log("sleep");
    pool_->affectNbIdleThreads(+1);
  }
  while(log("block") && pool_->waitForWork());
  log("ciao");
}


void
Minion::browseNode(PatriciaTreeNode* node, unsigned char keyLen)
{
  if (keyLen + node->getStrLength() > wordLen_ + maxDistance_)
  {
    //std::cout << "kick A" << std::endl;
    return; // No chance to do better
  }

  unsigned char oldKeyLen = keyLen;
  unsigned char toBeCopied = std::min(
    node->getStrLength(),
    wordLen_ + maxDistance_ - keyLen
    );
  memcpy(key_ + keyLen, treeData_ + node->getStrStart(), toBeCopied);
  keyLen += toBeCopied;


  // compute the distance
  unsigned char minDistance;
  unsigned char realDistance;
  calculateDistance(oldKeyLen, keyLen, &minDistance, &realDistance);


  if (minDistance > maxDistance_ + 2 && keyLen < wordLen_)
  {
    return; // No chance to match
  }

  //std::cerr << "pass" << std::endl << std::endl << std::endl;

  // Add to results if the node means a word

  //std::cout << "pass" << std::endl;
  if (realDistance <= maxDistance_)
  {
    size_t freq = node->getFrequency();
    if (freq > 0)
    {
      //std::cout << strlen(key_) << std::endl;
      std::string str(key_, keyLen);
      SearchResult result(str, realDistance, freq);
      pool_->resultListLock();
      collector_->push_back(result);
      pool_->resultListUnlock();
    }
  }



  unsigned char nbIdleThreads = pool_->getNbIdleThreads();
  unsigned char nbSons = node->sons_.size();
  for (
    std::list<PatriciaTreeNode*>::iterator it = node->sons_.begin();
    it != node->sons_.end();
    it++
    )
  {
    if (nbSons > 1 && nbIdleThreads > 0)
    {
      // Give the task to another thread if possible (mode tire-au-flanc)
      std::string key(key_, keyLen);
      pool_->submitTask(*it, key);
      nbIdleThreads--;
      //log("I submit");
    }
    else
    {
      //log("I go");
      browseNode(*it, keyLen);
      nbIdleThreads = pool_->getNbIdleThreads();
    }
    nbSons--;
  }
}


inline unsigned char min2(unsigned char a, unsigned char b)
{
  if (a > b)
    return b;
  else
    return a;
}

inline unsigned char min3(unsigned char a, unsigned char b, unsigned char c)
{
  return min2(a, min2(b, c));
}


void
Minion::calculateDistance(unsigned char oldKeyLen,
			  unsigned char keyLen,
			  unsigned char* minDistance,
			  unsigned char* realDistance)
{
  // Partial Damerau-Levenshtein distance
  for (unsigned char j = oldKeyLen + 1; j <= keyLen; ++j)
    for (unsigned char i = 1; i < cmpTableSize_; ++i)
    {
      unsigned char cost;
      // std::cerr << "i: " << i << " j: " << j << " w[i-1]='"
      // 		<< word_[i-1] << "' k[j-1]='" << key_[j-1] << "'" << std::endl;
      if (word_[i - 1] == key_[j - 1])
	cost = 0;
      else
	cost = 1;

      cmpTable_[i][j] = min3(
	(unsigned char)(cmpTable_[i - 1][j] + 1),     // deletion
	(unsigned char)(cmpTable_[i][j - 1] + 1),     // insertion
	(unsigned char)(cmpTable_[i - 1][j - 1] + cost)   // substitution
	);
      if(i > 1 && j > 1 &&
	 word_[i - 1] == key_[j - 2] && word_[i - 2] == key_[j - 1])
	cmpTable_[i][j] = min2(
	  (unsigned char)cmpTable_[i][j],
	  (unsigned char)(cmpTable_[i - 2][j - 2] + cost)   // transposition
	  );
    }
  // if (minDistance == NULL)
  // {
  //   std::cout << "hey " << oldKeyLen << std::endl;
  //   std::cout << "hey " << keyLen << std::endl;
  //   tableDisplay(std::cout, keyLen);
  // }

  // if (cmpTable_[1][1] == 0)
  // {
  //   std::cout << oldKeyLen << std::endl;
  //   std::cout << keyLen << std::endl;
  //   tableDisplay(std::cout, keyLen);
  // }
  if (minDistance != NULL)
    *minDistance = cmpTable_[keyLen][keyLen];
  if (realDistance != NULL)
    *realDistance = cmpTable_[wordLen_][keyLen];
}



void
Minion::browseNode0(PatriciaTreeNode* node, unsigned char keyLen)
{
  unsigned char nodeStrLength = node->getStrLength();
  if (keyLen + nodeStrLength > wordLen_)
    return; // No chance to fit

  if (strncmp(word_ + keyLen,
	      treeData_ + node->getStrStart(),
	      nodeStrLength))
    return; // The two strings are different


  memcpy(key_ + keyLen, treeData_ + node->getStrStart(), nodeStrLength);
  keyLen += nodeStrLength;

  if (keyLen == wordLen_)
  {
    size_t freq = node->getFrequency();
    if (freq > 0)
    {
      std::string str(key_, keyLen);
      SearchResult result(str, 0, freq);
      pool_->resultListLock();
      collector_->push_back(result);
      pool_->resultListUnlock();
    }
  }

  unsigned char nbIdleThreads = pool_->getNbIdleThreads();
  unsigned char nbSons = node->sons_.size();
  for (
    std::list<PatriciaTreeNode*>::iterator it = node->sons_.begin();
    it != node->sons_.end();
    it++
    )
  {
    if (nbSons > 1 && nbIdleThreads > 0)
    {
      // Give the task to another thread if possible (mode tire-au-flanc)
      std::string key(key_, keyLen);
      pool_->submitTask(*it, key);
      nbIdleThreads--;
      //log("I submit");
    }
    else
    {
      //log("I go");
      browseNode0(*it, keyLen);
      nbIdleThreads = pool_->getNbIdleThreads();
    }
    nbSons--;
  }
}