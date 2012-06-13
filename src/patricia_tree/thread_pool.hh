#ifndef THREAD_POOL_HH_
# define THREAD_POOL_HH_

# include <list>
# include <utility>
# include <pthread.h>

# include "minion.hh"

class PatriciaTreeNodeApp;


typedef std::pair<PatriciaTreeNodeApp*, std::string> nodeFetchTask;

class ThreadPool
{

  public:
    ThreadPool(unsigned int nbThreads);
    ~ThreadPool();

    void configure(const char* word,
		   unsigned int maxDistance,
		   const char* treeData,
		   std::list<SearchResult>* collector);

    void submitTask(PatriciaTreeNodeApp* node, std::string& prefix);

    void join();
    bool wannaQuit();

    bool todoListIsEmpty();
    nodeFetchTask* todoListPopTask();

    bool waitForWork();
    void minionJobStartedBroadcast();
    void waitForFinish();
    bool allMinionHaveFinished();

    unsigned char getNbIdleThreads();
    unsigned char updateNbIdleMinion();

    void resultListLock();
    void resultListUnlock();

    void log(std::string msg);
    void logLock();
    void logUnlock();

  private:
    const char* treeData_;

    std::list<pthread_t*> threads_;

    std::list<nodeFetchTask*> todoList_;

    std::list<Minion*> minions_;

    pthread_cond_t todoListNotEmpty_;
    pthread_mutex_t todoListNotEmptyMutex_;

    pthread_mutex_t resultListMutex_;

    pthread_mutex_t logMutex_;

    pthread_cond_t waitForMinionsStartingJob_;
    pthread_mutex_t waitForMinionsStartingJobMutex_;

    pthread_cond_t waitForFinish_;
    pthread_mutex_t waitForFinishMutex_;

    pthread_mutex_t todoListMutex_;

    unsigned char nbThreads_;
    unsigned char nbIdleThreads_;
    bool wannaQuit_;
    bool minionsHaveStarted_;
    char alignPadding_[4];
};

# include "thread_pool.hxx"

#endif /* !THREAD_POOL_HH_ */
