#ifndef THREAD_POOL_HH_
# define THREAD_POOL_HH_

# include <list>
# include <utility>
# include <pthread.h>

# include "minion.hh"

class PatriciaTreeNode;


typedef std::pair<PatriciaTreeNode*, std::string> nodeFetchTask;

class ThreadPool
{

  public:
    ThreadPool(unsigned int nbThread);
    ~ThreadPool();

    void configure(const char* word,
		   unsigned int maxDistance,
		   std::list<SearchResult>& collector);

    void submitTask(PatriciaTreeNode*, std::string&);


    nodeFetchTask* todoListPopTask();
    bool todoListIsEmpty();
    bool todoListIsEmpty_lock();
    void todoListLock();
    void todoListUnlock();
    pthread_cond_t* getTodoListCond();
    pthread_mutex_t* getTodoListCondMutex();
    bool getNbIdleThreads();

  private:
    std::list<pthread_t> threads_;
    unsigned int nbIdleThreads_;
    std::list<nodeFetchTask*> todoList_;
    pthread_mutex_t todoListMutex_;
    pthread_cond_t todoListEmpty_;
    pthread_mutex_t todoListEmptyMutex_;

    std::list<Minion*> minions_;
};

#endif /* !THREAD_POOL_HH_ */
