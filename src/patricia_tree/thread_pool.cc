#include "thread_pool.hh"
#include "patricia_tree_node.hh"

void* fetchQueue(void*);

ThreadPool::ThreadPool(unsigned int nbThread)
{
  nbIdleThreads_ = nbThread;
  pthread_mutex_init(&todoListMutex_, NULL);
  pthread_mutex_init(&todoListEmptyMutex_, NULL);
  todoListEmpty_ = PTHREAD_COND_INITIALIZER;

  for (unsigned int i = 0; i < nbThread; i++)
  {
    pthread_t thread;
    Minion* newMinion = new Minion(*this);
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
		      std::list<SearchResult>& collector)
{
  for(
    std::list<Minion*>::iterator it = minions_.begin();
    it != minions_.end();
    it++
    )
    (*it)->configure(word, maxDistance, collector);
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
  return task;
}


bool
ThreadPool::getNbIdleThreads()
{
  return nbIdleThreads_;
}


void
ThreadPool::submitTask(PatriciaTreeNode* node, std::string& str)
{
  nodeFetchTask* task = new nodeFetchTask(node, str);
  todoListLock();
  todoList_.push_back(task);
  todoListUnlock();
  pthread_cond_broadcast(&todoListEmpty_);
}


void* fetchQueue(void* argMinion)
{
  Minion* minion = (Minion*)argMinion;
  minion->run();
  return NULL;
}





