#include "minion.hh"
#include "thread_pool.hh"

Minion::Minion(ThreadPool& pool)
  : pool_ (pool)
{
  word_ = NULL;
  cmpTable_ = NULL;
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
Minion::configure(const char* word,
	       unsigned int maxDistance,
	       std::list<SearchResult>& collector)
{
  word_ = word;
  maxDistance_ = maxDistance;
  collector_ = &collector;

  // Redim the table for distance calculation
  wordLen_ = strlen(word);
  deleteTable();
  cmpTableSize_ = wordLen_ + maxDistance_;
  cmpTable_ = new char*[cmpTableSize_];
  for (size_t i = 0; i < cmpTableSize_; i++)
    cmpTable_[i] = new char[cmpTableSize_];
}


bool
Minion::getATask()
{
  pool_.todoListLock();
  if (pool_.todoListIsEmpty()) // Ils nous volent notre travail !
  {
    pool_.todoListUnlock();
    return false;
  }
  nodeFetchTask* task = pool_.todoListPopTask();
  pool_.todoListUnlock();

  PatriciaTreeNode* node = task->first;
  std::string str = task->second;
  delete task;

  // FIXME: Descendre dans le noeud et faire les calculs !
  std::cerr << "Oui mon seigneuuur" << std::endl;


  return true;
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
    while(getATask());
  }
}
