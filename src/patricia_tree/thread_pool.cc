#include "thread_pool.hh"
#include "patricia_tree_node_app.hh"

void* fetchQueue(void*);

ThreadPool::ThreadPool(unsigned int nbThreads)
  : nbThreads_ (nbThreads),
    nbIdleThreads_ (nbThreads),
    wannaQuit_(false)
{
  pthread_mutex_init(&todoListMutex_, NULL);

  pthread_mutex_init(&todoListNotEmptyMutex_, NULL);
  pthread_cond_init(&todoListNotEmpty_, NULL);

  pthread_mutex_init(&resultListMutex_, NULL);

  pthread_mutex_init(&logMutex_, NULL);

  pthread_mutex_init(&waitForMinionsStartingJobMutex_, NULL);
  pthread_cond_init(&waitForMinionsStartingJob_, NULL);

  pthread_mutex_init(&waitForFinishMutex_, NULL);
  pthread_cond_init(&waitForFinish_, NULL);


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

  pthread_mutex_destroy(&waitForMinionsStartingJobMutex_);
  pthread_cond_destroy(&waitForMinionsStartingJob_);

  pthread_mutex_destroy(&waitForFinishMutex_);
  pthread_cond_destroy(&waitForFinish_);


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
		      std::list<SearchResult>* collector)
{
  log("config start");
  minionsHaveStarted_ = false;
  treeData_ = treeData;
  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
  {
    (*it)->configure(word, maxDistance, treeData, collector);
  }
  log("config end");
}

bool
ThreadPool::todoListIsEmpty()
{
  bool isEmpty;
  pthread_mutex_lock(&todoListMutex_);
  isEmpty = todoList_.empty();
  pthread_mutex_unlock(&todoListMutex_);
  return isEmpty;
}



nodeFetchTask*
ThreadPool::todoListPopTask()
{
  pthread_mutex_lock(&todoListMutex_);
  if (todoList_.empty())
  {
    pthread_mutex_unlock(&todoListMutex_);
    return NULL;
  }
  nodeFetchTask* task = todoList_.front();
  todoList_.pop_front();
  pthread_mutex_unlock(&todoListMutex_);
  return task;
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
ThreadPool::submitTask(PatriciaTreeNodeApp* node, std::string& prefix)
{
  //log("task submitted");
  nodeFetchTask* task = new nodeFetchTask(node, prefix);
  pthread_mutex_lock(&todoListNotEmptyMutex_);

  pthread_mutex_lock(&todoListMutex_);
  todoList_.push_back(task);
  pthread_mutex_unlock(&todoListMutex_);

  pthread_cond_broadcast(&todoListNotEmpty_);
  pthread_mutex_unlock(&todoListNotEmptyMutex_);
}


bool
ThreadPool::waitForWork()
{
  // Claim it has finished
  pthread_mutex_lock(&waitForFinishMutex_);
  pthread_cond_broadcast(&waitForFinish_);
  pthread_mutex_unlock(&waitForFinishMutex_);

  // Wait for some new work
  pthread_mutex_lock(&todoListNotEmptyMutex_);
  while (!wannaQuit_ && todoListIsEmpty())
  {
    pthread_cond_wait(&todoListNotEmpty_, &todoListNotEmptyMutex_);
  }
  pthread_mutex_unlock(&todoListNotEmptyMutex_);
  return true;
}



void
ThreadPool::minionJobStartedBroadcast()
{
  updateNbIdleMinion();
  pthread_mutex_lock(&waitForMinionsStartingJobMutex_);
  minionsHaveStarted_ = true;
  pthread_cond_broadcast(&waitForMinionsStartingJob_);
  pthread_mutex_unlock(&waitForMinionsStartingJobMutex_);
}


void
ThreadPool::waitForFinish()
{
  // Wait for any thread to start the job
  //log("waiting for a minion to start...");
  pthread_mutex_lock(&waitForMinionsStartingJobMutex_);
  while(!minionsHaveStarted_)
  {
    //log("cond waitForMinionsStartingJob_");
    pthread_cond_wait(&waitForMinionsStartingJob_,
		      &waitForMinionsStartingJobMutex_);
    //log("cond waitForMinionsStartingJob_ passed");
  }
  pthread_mutex_unlock(&waitForMinionsStartingJobMutex_);
  //log("A minion started");

  // Loop until everybody has finished
  //log("waiting for everybody to finish...");
  pthread_mutex_lock(&waitForFinishMutex_);
  while(!allMinionHaveFinished() || !todoListIsEmpty())
  {
    //log("cond waitForFinish_");
    pthread_cond_wait(&waitForFinish_, &waitForFinishMutex_);
    //log("cond waitForFinish_ passed");
  }
  //log("waitForFinishMutex_ unlocked");
  pthread_mutex_unlock(&waitForFinishMutex_);
  //log("Everybody finished");
}


bool
ThreadPool::allMinionHaveFinished()
{
  return updateNbIdleMinion() == nbThreads_;
}



unsigned char
ThreadPool::updateNbIdleMinion()
{
  unsigned char nbIdle = 0;
  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
  {
    if ((*it)->isIdle())
      nbIdle++;
  }

  nbIdleThreads_ = nbIdle;
  return nbIdleThreads_;
}



void
ThreadPool::join()
{
  pthread_mutex_lock(&todoListNotEmptyMutex_);

  wannaQuit_ = true;
  //int i = 1;
  log("waiting for join");

  pthread_cond_broadcast(&todoListNotEmpty_);
  pthread_mutex_unlock(&todoListNotEmptyMutex_);

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


