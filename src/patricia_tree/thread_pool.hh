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
    ThreadPool(unsigned int nbThreads,
	       pthread_cond_t* parsingDone,
	       pthread_mutex_t* parsingDoneMutex);
    ~ThreadPool();

    void configure(const char* word,
		   unsigned int maxDistance,
		   const char* treeData,
		   std::list<SearchResult>& collector);

    void configureWaitMutex();
    void setConfigured(bool val);
    void submitTask(PatriciaTreeNode* node, std::string& prefix);

    void join();
    bool wannaQuit();

    nodeFetchTask* todoListPopTask();
    bool todoListIsNotEmpty();
    void todoListLock();
    void todoListUnlock();

    bool waitForWork();

    bool getNbIdleThreads();
    void affectNbIdleThreads(char i);

    void resultListLock();
    void resultListUnlock();

    void log(std::string msg);
    void logLock();
    void logUnlock();

  private:
    unsigned int nbThreads_;
    const char* treeData_;
    bool wannaQuit_;
    std::list<pthread_t*> threads_;
    unsigned int nbIdleThreads_;
    std::list<nodeFetchTask*> todoList_;
    pthread_mutex_t todoListMutex_;

    pthread_cond_t todoListNotEmpty_;
    pthread_mutex_t todoListNotEmptyMutex_;

    pthread_mutex_t resultListMutex_;

    pthread_cond_t* parsingDone_;
    pthread_mutex_t* parsingDoneMutex_;

    pthread_mutex_t logMutex_;

    pthread_mutex_t nbIdleThreadMutex_;

    pthread_cond_t minionConfigured_;
    pthread_mutex_t minionConfiguredMutex_;
    bool isConfigured_;

    std::list<Minion*> minions_;
};

# include "thread_pool.hxx"

#endif /* !THREAD_POOL_HH_ */
