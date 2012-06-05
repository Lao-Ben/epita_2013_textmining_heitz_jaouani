#include <cstdlib>
#include <algorithm>

#include "minion.hh"
#include "thread_pool.hh"

Minion::Minion(ThreadPool& pool)
  : pool_ (pool)
{
  word_ = NULL;
  cmpTable_ = NULL;
  key_ = NULL;
}

Minion::~Minion()
{
  deleteTable();
}


void
Minion::deleteTable()
{
  if (cmpTable_ != NULL)
  {
    for (size_t i = 0; i < cmpTableSize_; i++)
      delete[] cmpTable_[i];
  }
  delete[] cmpTable_;
}


void
Minion::tableDisplay(std::ostream& out, size_t keyLen)
{
  size_t i;
  size_t j;
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
      out << "| "
	  << (int)cmpTable_[i][j]
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

  key_ = new char[cmpTableSize_];
  for (size_t i = 0; i < cmpTableSize_; i++)
    key_[i] = ' ';


  cmpTable_ = new unsigned char*[cmpTableSize_];
  for (size_t i = 0; i < cmpTableSize_; i++)
  {
    cmpTable_[i] = new unsigned char[cmpTableSize_];
    cmpTable_[0][i] = i;
    cmpTable_[i][0] = i;
    for(size_t j= 1; j < cmpTableSize_; j++)
      if (i != 0)
	cmpTable_[i][j] = 0;
  }
}


bool
Minion::getATask()
{
  //std::cerr << "\t\tget a task" << std::endl;
  pool_.todoListLock();
  if (pool_.todoListIsEmpty())
  {
    // Ils nous volent notre travail !
    //std::cerr << "\t\t\tempty :(" << std::endl;
    pool_.todoListUnlock();
    return false;
  }
  // Cool du boulot !
  nodeFetchTask* task = pool_.todoListPopTask();
  pool_.todoListUnlock();
  reInitKey(task->second);

  //std::cerr << "\t\t\tbrowse" << std::endl;
  browseNode(task->first, task->second.size());
  delete task;

  return true;
}

void
Minion::reInitKey(std::string& key)
{
  key.copy(key_, key.size());
}

void
Minion::run()
{
  // Infinite loop
  while(0 == pthread_cond_wait(
	  pool_.getTodoListCond(),
	  pool_.getTodoListCondMutex()))
  {
    // Wake up
    // Is there still something to do ?
    //std::cerr << "\t\twork" << std::endl;
    pool_.affectNbIdleThreads(-1);
    while(getATask())
    {
      //std::cerr << "\t\ttask done" << std::endl;
    }
    if (pool_.wannaQuit())
    {
      //std::cerr << "pool wanna break" << std::endl;
      break;
    }
    //std::cerr << "\t\tsleep" << std::endl;
    pool_.affectNbIdleThreads(+1);
  }
  //std::cerr << "ciao" << std::endl;
}


void
Minion::browseNode(PatriciaTreeNode* node, size_t keyLen)
{
  // std::cerr <<  "node: " << node->getStr(treeData_) << std::endl;
  // std::cerr <<  "keyLen: " << keyLen << std::endl;
  // std::cerr.write(key_, keyLen);
  // std::cerr << std::endl;
  // Append the key to the buffer

  if (keyLen + node->getStrLength() > wordLen_ + maxDistance_)
  {
    return; // No chance to do better
  }

  size_t oldKeyLen = keyLen;
  int toBeCopied = std::min(
    node->getStrLength(),
    wordLen_ + maxDistance_ - keyLen
    );
  memcpy(key_ + keyLen, treeData_ + node->getStrStart(), toBeCopied);
  keyLen += toBeCopied;


  // compute the distance
  size_t minDistance;
  size_t realDistance;
  calculateDistance(oldKeyLen, keyLen, &minDistance, &realDistance);
  // std::cout << "word:      '" << word_ << "'" << std::endl
  // 	    << "key:       '";
  // std::cout.write(key_, keyLen)
  //   << "'"<< std::endl
  //   << "minDistance:    " << minDistance << std::endl
  //    << "realDistance:   " << realDistance << std::endl;


  if (minDistance > maxDistance_ + 1 && keyLen < wordLen_)
  {
    //std::cout << "kick B" << std::endl;
    return; // No chance to match
  }

  //std::cerr << "pass" << std::endl << std::endl << std::endl;


  // Add to results if the node means a word
  if (realDistance <= maxDistance_)
  {
    size_t freq = node->getFrequency();
    if (freq > 0)
    {
//      std::cout << strlen(key_) << std::endl;
      std::string str(key_, keyLen);
//      if (!strcmp(key_, "lolo"))
//	std::cout << "ADD: " << str << std::endl;
///	std::cout << freq << std::endl;
//	std::cout << std::endl;
      SearchResult result(str, realDistance, freq);
      pool_.resultListLock();
      collector_->push_back(result);
      pool_.resultListUnlock();
    }
  }


  unsigned int nbIdleThreads = pool_.getNbIdleThreads();
  //std::cerr << "nbSons: " << node->sons_.size() << std::endl;
  for (
    std::list<PatriciaTreeNode*>::iterator it = node->sons_.begin();
    it != node->sons_.end();
    it++
    )
  {
    if (nbIdleThreads > 0)
    {
      // Give the task to another thread if possible (mode tire-au-flanc)
      std::string key(key_, keyLen);
      //std::cout << "submit" << std::endl;
      pool_.submitTask(*it, key);
      nbIdleThreads--;
    }
    else
    {
      //std::cout << "go" << std::endl;
      browseNode(*it, keyLen);
      nbIdleThreads = pool_.getNbIdleThreads();
    }
  }
  //std::cout << std::endl<< std::endl;
}


void
Minion::calculateDistance(size_t oldKeyLen,
			  size_t keyLen,
			  size_t* minDistance,
			  size_t* realDistance)
{
  // Partial Damerau-Levenshtein distance
  unsigned char cost;
  // std::cerr << "oldKeyLen: " << oldKeyLen << std::endl;
  // std::cerr << "keyLen: " << keyLen << std::endl;
  for (size_t j = oldKeyLen + 1; j <= keyLen; j++)
    for (size_t i = 1; i < cmpTableSize_; i++)
    {
      // std::cerr << "i: " << i << " j: " << j << " w[i-1]='"
      // 		<< word_[i-1] << "' k[j-1]='" << key_[j-1] << "'" << std::endl;
      if (word_[i - 1] == key_[j - 1])
	cost = 0;
      else
	cost = 1;

      cmpTable_[i][j] = std::min(
	(size_t)(cmpTable_[i - 1][j] + 1),     // deletion
	std::min(
	  (size_t)(cmpTable_[i][j - 1] + 1),     // insertion
	  (size_t)(cmpTable_[i - 1][j - 1] + cost)   // substitution
	  )
	);
      if(i > 1 && j > 1 &&
	 word_[i - 1] == key_[j - 2] && word_[i - 2] == key_[j - 1])
	cmpTable_[i][j] = std::min(
	  (size_t)cmpTable_[i][j],
	  (size_t)(cmpTable_[i - 2][j - 2] + cost)   // transposition
	  );
    }
//  tableDisplay(std::cout, keyLen);
  *minDistance = cmpTable_[keyLen][keyLen];
  *realDistance = cmpTable_[wordLen_][keyLen];
}
