#include "thread_pool.hh"
#include "patricia_tree_node.hh"

void* fetchQueue(void*);

ThreadPool::ThreadPool(unsigned int nbThreads)
  : nbThreads_ (nbThreads),
    wannaQuit_(false),
    nbIdleThreads_ (nbThreads)
{
  pthread_mutex_init(&todoListMutex_, NULL);
  pthread_mutex_init(&todoListEmptyMutex_, NULL);
  pthread_mutex_init(&resultListMutex_, NULL);
  todoListEmpty_ = PTHREAD_COND_INITIALIZER;

  for (unsigned int i = 0; i < nbThreads_; i++)
  {
    pthread_t thread;
    Minion* newMinion = new Minion(*this);
    minions_.push_back(newMinion);
    pthread_create(&thread, NULL, fetchQueue, newMinion);
    threads_.push_back(thread);
  }
}


ThreadPool::~ThreadPool()
{
  for(
    std::list<pthread_t>::iterator it = threads_.begin();
    it != threads_.end();
    it++
    )
    pthread_cancel(*it);
  pthread_mutex_destroy(&todoListMutex_);
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


pthread_cond_t*
ThreadPool::getTodoListCond()
{
  return &todoListEmpty_;
}

pthread_mutex_t*
ThreadPool::getTodoListCondMutex()
{
  return &todoListEmptyMutex_;
}


nodeFetchTask*
ThreadPool::todoListPopTask()
{
  nodeFetchTask* task = todoList_.front();
  todoList_.pop_front();
  //std::cerr << "idle: " << nbIdleThreads_ <<  " / " << nbThreads_ << std::endl;
  return task;
}


bool
ThreadPool::getNbIdleThreads()
{
  //std::cerr << "idle: " << nbIdleThreads_ <<  " / " << nbThreads_ << std::endl;
  return nbIdleThreads_;
}

void
ThreadPool::affectNbIdleThreads(char i)
{
  //std::cerr << (nbIdleThreads_ + 1) <<  " / " << nbThreads_ << std::endl;
  nbIdleThreads_ += i;
  if (nbIdleThreads_ == nbThreads_)
  {
    //std::cerr << "parsing done !" << std::endl;
    pthread_cond_broadcast(parsingDone_);
  }
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
ThreadPool::setParsingDoneCond(pthread_cond_t* parsingDone)
{
  parsingDone_ = parsingDone;
}

void
ThreadPool::submitTask(PatriciaTreeNode* node, std::string& prefix)
{
  //std::cerr << "idle: " << nbIdleThreads_ <<  " / " << nbThreads_ << std::endl;
  nodeFetchTask* task = new nodeFetchTask(node, prefix);
  todoListLock();
  todoList_.push_back(task);
  todoListUnlock();
  pthread_cond_broadcast(&todoListEmpty_);
}


void
ThreadPool::join()
{
  wannaQuit_ = true;
  pthread_cond_broadcast(&todoListEmpty_);
  for(
    std::list<pthread_t>::iterator it = threads_.begin();
    it != threads_.end();
    it++
    )
    pthread_join(*it, NULL);
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

