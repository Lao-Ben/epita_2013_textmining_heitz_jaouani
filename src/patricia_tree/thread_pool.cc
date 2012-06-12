#include "thread_pool.hh"
#include "patricia_tree_node.hh"

void* fetchQueue(void*);

ThreadPool::ThreadPool(unsigned int nbThreads,
		       pthread_cond_t* parsingDone,
		       pthread_mutex_t* parsingDoneMutex)
  : nbThreads_ (nbThreads),
    wannaQuit_(false),
    nbIdleThreads_ (nbThreads)
{
  pthread_mutex_init(&todoListMutex_, NULL);
  pthread_mutex_init(&todoListNotEmptyMutex_, NULL);
  pthread_mutex_init(&resultListMutex_, NULL);
  pthread_mutex_init(&logMutex_, NULL);
  pthread_mutex_init(&nbIdleThreadMutex_, NULL);
  pthread_mutex_init(&minionConfiguredMutex_, NULL);
  pthread_cond_init(&minionConfigured_, NULL);

  isConfigured_ = false;

  pthread_cond_init(&todoListNotEmpty_, NULL);

  parsingDone_ = parsingDone;
  parsingDoneMutex_ = parsingDoneMutex;

  for (unsigned int i = 0; i < nbThreads_; i++)
  {
    pthread_t* newThread = new pthread_t();
    Minion* newMinion = new Minion(this, i + 1);
    if (newMinion == NULL)
    {
      std::cerr << "new Minion FAIL" << std::endl;
    }
    minions_.push_back(newMinion);
    pthread_create(newThread, NULL, fetchQueue, newMinion);
    threads_.push_back(newThread);
  }
}


ThreadPool::~ThreadPool()
{
  pthread_mutex_destroy(&todoListMutex_);

  pthread_cond_destroy(&todoListNotEmpty_);
  pthread_mutex_destroy(&todoListNotEmptyMutex_);

  pthread_mutex_destroy(&resultListMutex_);

  pthread_mutex_destroy(&logMutex_);

  pthread_mutex_destroy(&nbIdleThreadMutex_);

  pthread_cond_destroy(&minionConfigured_);
  pthread_mutex_destroy(&minionConfiguredMutex_);

  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
  {
    delete (*it);
  }

  for(
    std::list<pthread_t*>::iterator it = threads_.begin();
    it != threads_.end();
    it++
    )
  {
    delete (*it);
  }
}


void
ThreadPool::configure(const char* word,
		      unsigned int maxDistance,
		      const char* treeData,
		      std::list<SearchResult>& collector)
{
  treeData_ = treeData;
  pthread_mutex_lock(&minionConfiguredMutex_);
  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
  {
    (*it)->configure(word, maxDistance, treeData, collector);
  }
  isConfigured_ = true;
  pthread_cond_broadcast(&minionConfigured_);
  pthread_mutex_unlock(&minionConfiguredMutex_);
}

void
ThreadPool::configureWaitMutex()
{
  while (!isConfigured_)
  {
    pthread_mutex_lock(&minionConfiguredMutex_);
    pthread_cond_wait(&minionConfigured_, &minionConfiguredMutex_);
    pthread_mutex_unlock(&minionConfiguredMutex_);
  }
}


nodeFetchTask*
ThreadPool::todoListPopTask()
{
  nodeFetchTask* task = todoList_.front();
  todoList_.pop_front();
  return task;
}


bool
ThreadPool::waitForWork()
{
  pthread_mutex_lock(&todoListMutex_);
  pthread_cond_wait(&todoListNotEmpty_, &todoListMutex_);
  pthread_mutex_unlock(&todoListMutex_);
  return true;
}



void
ThreadPool::affectNbIdleThreads(char i)
{
  pthread_mutex_lock(&nbIdleThreadMutex_);
  nbIdleThreads_ += i;
  pthread_mutex_unlock(&nbIdleThreadMutex_);
  std::string msg;
  msg += (nbIdleThreads_ + '0');
  msg += " idle";
  if (nbIdleThreads_ == nbThreads_)
  {
    msg += " (parsing done !)";
    pthread_mutex_lock(parsingDoneMutex_);
    pthread_cond_broadcast(parsingDone_);
    pthread_mutex_unlock(parsingDoneMutex_);
  }
  log(msg);
}


void
ThreadPool::log(std::string msg)
{
  return;
  logLock();
  std::cout << "Pool : "
	    << msg << std::endl;
  logUnlock();
}



void
ThreadPool::submitTask(PatriciaTreeNode* node, std::string& prefix)
{
  //log("task submitted");
  nodeFetchTask* task = new nodeFetchTask(node, prefix);
  todoListLock();
  todoList_.push_back(task);
  todoListUnlock();
  pthread_mutex_lock(&todoListNotEmptyMutex_);
  pthread_cond_broadcast(&todoListNotEmpty_);
  pthread_mutex_unlock(&todoListNotEmptyMutex_);
}


void
ThreadPool::join()
{
  wannaQuit_ = true;
  //int i = 1;
  log("waiting for join");

  pthread_mutex_lock(&todoListNotEmptyMutex_);
  pthread_cond_broadcast(&todoListNotEmpty_);
  pthread_mutex_unlock(&todoListNotEmptyMutex_);

  pthread_mutex_lock(&minionConfiguredMutex_);
  isConfigured_ = true;
  pthread_cond_broadcast(&minionConfigured_);
  pthread_mutex_unlock(&minionConfiguredMutex_);

  for(
    std::list<pthread_t*>::iterator it = threads_.begin();
    it != threads_.end();
    it++
    )
  {
    //std::cout << "waiting thread " << i << "..." << std::endl;
    pthread_join(*(*it), NULL);
    //std::cout << "thread " << i << " termine" << std::endl;
    //i++;
  }
}


void* fetchQueue(void* argMinion)
{
  Minion* minion = (Minion*)argMinion;
  minion->run();
  return NULL;
}

