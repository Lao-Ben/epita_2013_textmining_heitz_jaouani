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
  todoListMutex_ = PTHREAD_MUTEX_INITIALIZER;
  todoListEmptyMutex_ = PTHREAD_MUTEX_INITIALIZER;
  resultListMutex_ = PTHREAD_MUTEX_INITIALIZER;
  logMutex_ = PTHREAD_MUTEX_INITIALIZER;
  minionConfigureMutex_ = PTHREAD_MUTEX_INITIALIZER;
  nbIdleThreadMutex_ = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&minionConfigureMutex_);

  todoListEmpty_ = PTHREAD_COND_INITIALIZER;

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

  pthread_cond_destroy(&todoListEmpty_);
  pthread_mutex_destroy(&todoListEmptyMutex_);

  pthread_mutex_destroy(&resultListMutex_);

  pthread_mutex_destroy(&logMutex_);

  pthread_mutex_destroy(&nbIdleThreadMutex_);

  pthread_mutex_destroy(&minionConfigureMutex_);

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
  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
  {
    (*it)->configure(word, maxDistance, treeData, collector);
  }
  pthread_mutex_unlock(&minionConfigureMutex_);
}

void
ThreadPool::configureWaitMutex()
{
  pthread_mutex_lock(&minionConfigureMutex_);
  pthread_mutex_unlock(&minionConfigureMutex_);
}



void
ThreadPool::todoListLock()
{
  pthread_mutex_lock(&todoListMutex_);
}

void
ThreadPool::todoListUnlock()
{
  pthread_mutex_unlock(&todoListMutex_);
}

bool
ThreadPool::todoListIsEmpty()
{
  return todoList_.empty();
}

bool
ThreadPool::todoListIsEmpty_lock()
{
  todoListLock();
  bool empty = todoList_.empty();
  todoListUnlock();
  return empty;
}

bool
ThreadPool::waitForWork()
{
  pthread_mutex_lock(&todoListMutex_);
  pthread_cond_wait(&todoListEmpty_, &todoListMutex_);
  pthread_mutex_unlock(&todoListMutex_);
  return true;
}




nodeFetchTask*
ThreadPool::todoListPopTask()
{
  nodeFetchTask* task = todoList_.front();
  todoList_.pop_front();
  pthread_cond_broadcast(&todoListEmpty_);
  return task;
}


bool
ThreadPool::getNbIdleThreads()
{
  return nbIdleThreads_;
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
ThreadPool::resultListLock()
{
  pthread_mutex_lock(&resultListMutex_);
}

void
ThreadPool::resultListUnlock()
{
  pthread_mutex_unlock(&resultListMutex_);
}


void
ThreadPool::logLock()
{
  pthread_mutex_lock(&logMutex_);
}

void
ThreadPool::logUnlock()
{
  pthread_mutex_unlock(&logMutex_);
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
  pthread_mutex_lock(&todoListEmptyMutex_);
  pthread_cond_broadcast(&todoListEmpty_);
  pthread_mutex_unlock(&todoListEmptyMutex_);
}


void
ThreadPool::join()
{
  wannaQuit_ = true;
  //int i = 1;
  log("waiting for join");

  pthread_mutex_lock(&todoListEmptyMutex_);
  pthread_cond_broadcast(&todoListEmpty_);
  pthread_mutex_unlock(&todoListEmptyMutex_);

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

bool
ThreadPool::wannaQuit()
{
  return wannaQuit_;
}


void* fetchQueue(void* argMinion)
{
  Minion* minion = (Minion*)argMinion;
  minion->run();
  return NULL;
}

