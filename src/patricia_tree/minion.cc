#include <cstdlib>
#include <algorithm>
#include "minion.hh"
#include "thread_pool.hh"



Minion::Minion(ThreadPool* pool,
	       unsigned char num)
  : pool_ (pool)
{
  word_ = NULL;

  cmpTable_ = NULL;
  cmpTableSize_ = 0;
  cmpTableActualSize_ = 0;

  key_ = NULL;
  keyBufferSize_ = 0;

  num_ = num;

  pthread_mutex_init(&configure_, NULL);

  isIdle_ = true;
}


Minion::~Minion()
{
  if (cmpTable_ != NULL)
  {
    for (unsigned int i = 0; i < cmpTableActualSize_; i++)
      delete[] cmpTable_[i];
    delete[] cmpTable_;
  }

  if (key_ != NULL)
    delete[] key_;

  pthread_mutex_destroy(&configure_);
}



void
Minion::tableDisplay(std::ostream& out, unsigned char keyLen)
{
  unsigned char i;
  unsigned char j;
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
      if ((j + maxDistance_ >= i && i + maxDistance_ >= j)
	  || i == 0 || j == 0
	)
      {
	if (cmpTable_[i][j] < 10)
	  out << " ";
	out << (int)cmpTable_[i][j]
	    << " ";
      }
      else
	out << "   ";
    }
    if (j < cmpTableSize_)
    {
      out << "| ";
      for (; j < cmpTableSize_; j++)
      {
	out << "  | ";
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
  //return true;
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
Minion::configure(const char* word,
		  unsigned char maxDistance,
		  const char* treeData,
		  std::list<SearchResult>* collector)
{
  pthread_mutex_lock(&configure_);
  //log("being configured...");
  word_ = word;
  maxDistance_ = maxDistance;
  collector_ = collector;
  treeData_ = treeData;

  wordLen_ = strlen(word);

  // Redim the key buffer if too small
  unsigned char keyBufferSize = wordLen_ + maxDistance_;
  if (keyBufferSize > keyBufferSize_)
  {
    if (key_ != NULL)
      delete[] key_;
    key_ = new char[keyBufferSize];
    keyBufferSize_ = keyBufferSize;
  }


  // If maxDistance is at least, we need a table
  if (maxDistance >= 1)
  {
    // Redim the distance calculation table if too small
    cmpTableSize_ = wordLen_ + maxDistance_ + 1;
    if (cmpTableSize_ > cmpTableActualSize_)
    {
      if (cmpTable_ != NULL)
      {
	for (unsigned char i = 0; i < cmpTableActualSize_; i++)
	  delete[] cmpTable_[i];
	delete[] cmpTable_;
      }
      cmpTable_ = new unsigned char*[cmpTableSize_];
      for (unsigned char i = 0; i < cmpTableSize_; i++)
      {
	cmpTable_[i] = new unsigned char[cmpTableSize_];
	cmpTable_[0][i] = i;
	cmpTable_[i][0] = i;
	for (unsigned char j = 1; j < cmpTableSize_; j++)
	  if (i != 0)
	    cmpTable_[i][j] = 0;
      }
      cmpTableActualSize_ = cmpTableSize_;
    }
  }

  //log("configuration finished...");
  pthread_mutex_unlock(&configure_);
}


bool
Minion::getATask()
{
  //log("getatask");
  nodeFetchTask* task = NULL;
  if (NULL == (task = pool_->todoListPopTask()))
  {
    // Ils nous volent notre travail !
    return false;
  }
  // Cool du boulot !

  pthread_mutex_lock(&configure_);
  if (maxDistance_ == 0)
  {
    task->second.copy(key_, task->second.size());
    browseNode0(task->first, task->second.size());
  }
  else
  {
    task->second.copy(key_, task->second.size());
    calculateDistance(0, task->second.size(), NULL, NULL);
    browseNode(task->first, task->second.size());
  }
  pthread_mutex_unlock(&configure_);

  delete task;
  return true;
}



void
Minion::run()
{
  //log("Let's Go !");
  // Infinite loop
  do
  {
    // Wake up
    if (pool_->wannaQuit())
    {
      //log("pool wanna break (1)");
      break;
    }
    // Is there still something to do ?
    //log("work");
    isIdle_ = false;
    pool_->minionJobStartedBroadcast();
    while(getATask())
    {
    }
    if (pool_->wannaQuit())
    {
      //log("pool wanna break (2)");
      break;
    }
    isIdle_ = true;
    //log("sleep");
  }
  while(pool_->waitForWork());

  //log("ciao");
}


void
Minion::browseNode(PatriciaTreeNodeApp* node, unsigned char keyLen)
{
  if (keyLen + node->getStrLength() > wordLen_ + maxDistance_)
  {
    return; // No chance to do better
  }

  unsigned char oldKeyLen = keyLen;
  unsigned char toBeCopied = min2(
    node->getStrLength(),
    wordLen_ + maxDistance_ - keyLen
    );
  memcpy(key_ + keyLen, treeData_ + node->getStrStart(), toBeCopied);
  keyLen += toBeCopied;


  // compute the distance
  unsigned char minDistance;
  unsigned char realDistance;
  calculateDistance(oldKeyLen, keyLen, &minDistance, &realDistance);


  if (minDistance > 2 * maxDistance_)
  {
    return; // No chance to match
  }

  // Add to results if the node means a word

  if (keyLen >= wordLen_ - maxDistance_ && realDistance <= maxDistance_)
  {
    unsigned int freq = node->getFrequency();
    if (freq > 0)
    {
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
    std::vector<PatriciaTreeNodeApp*>::iterator it = node->sons_.begin();
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
  return;
}

void
Minion::calculateDistance(unsigned char oldKeyLen,
			  unsigned char keyLen,
			  unsigned char* minDistance,
			  unsigned char* realDistance)
{
  // Partial Damerau-Levenshtein distance
  unsigned char iStart;
  if (oldKeyLen < maxDistance_ + 1)
    iStart = 1;
  else
    iStart = oldKeyLen - maxDistance_;

  unsigned char iEnd = min2(cmpTableSize_, keyLen + maxDistance_ + 1);

  for (unsigned char j = oldKeyLen + 1; j <= keyLen; ++j)
    for (unsigned char i = iStart; i < iEnd; ++i)
    {
      unsigned char cost;
      if (word_[i - 1] == key_[j - 1])
	cost = 0;
      else
	cost = 1;

      cmpTable_[i][j] = cmpTable_[i - 1][j - 1] + cost;

      // We only calculate the diagonal
      if (i + maxDistance_ > j)
	cmpTable_[i][j] = min2(cmpTable_[i][j], cmpTable_[i - 1][j] + 1);

      if (j + maxDistance_ > i)
	cmpTable_[i][j] = min2(cmpTable_[i][j], cmpTable_[i][j - 1] + 1);

      if(i > 1 && j > 1 &&
	 word_[i - 1] == key_[j - 2] && word_[i - 2] == key_[j - 1])
	cmpTable_[i][j] = min2(
	  (unsigned char)cmpTable_[i][j],
	  (unsigned char)(cmpTable_[i - 2][j - 2] + cost)   // transposition
	  );
    }
  if (minDistance != NULL)
    *minDistance = cmpTable_[keyLen][keyLen];
  if (realDistance != NULL)
    *realDistance = cmpTable_[wordLen_][keyLen];
}



void
Minion::browseNode0(PatriciaTreeNodeApp* node, unsigned char keyLen)
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
    unsigned int freq = node->getFrequency();
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
    std::vector<PatriciaTreeNodeApp*>::iterator it = node->sons_.begin();
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
    }
    else
    {
      browseNode0(*it, keyLen);
      nbIdleThreads = pool_->getNbIdleThreads();
    }
    nbSons--;
  }
}
