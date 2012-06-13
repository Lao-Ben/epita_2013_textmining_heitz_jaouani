#ifndef THREAD_POOL_HH_
# define THREAD_POOL_HH_

# include <list>
# include <utility>
# include <pthread.h>

# include "minion.hh"

class PatriciaTreeNodeApp;

/**
 * Tasks stored in the todolist. Composed by :
 *   - A node to browse
 *   - A prefix keyword
 */

typedef std::pair<PatriciaTreeNodeApp*, std::string> nodeFetchTask;

/**
 * The thread pool stacks the browsing tasks and launches the minions (threads)
 */

class ThreadPool
{

  public:

    /**
     * The constructor takes the number of threads to launch
     */
    ThreadPool(unsigned int nbThreads);

    /**
     * Default destructor
     */
    ~ThreadPool();

    /**
     * Configure all the minions in order to search for a new word
     */
    void configure(const char* word,
		   unsigned int maxDistance,
		   const char* treeData,
		   std::list<SearchResult>* collector);

    /**
     * Push a new task on the todolist
     */
    void submitTask(PatriciaTreeNodeApp* node, std::string& prefix);

    /**
     * Wait for all the minions to join the main thread
     */
    void join();

    /**
     * Return whether or not the pool wants the minions to join
     */
    bool wannaQuit();

    /**
     * Return whether or not the todolist is empty
     */
    bool todoListIsEmpty();

    /**
     * Thread-safe pop the first task of the todolist
     */
    nodeFetchTask* todoListPopTask();

    /**
     * Block the thread and wait for a work signal
     */
    bool waitForWork();

    /**
     * Broadcast a signal to the pool when a minion has started
     */
    void minionJobStartedBroadcast();

    /**
     * Wait for all minions to finish their tasks
     */
    void waitForFinish();

    /**
     * Check if every minion is idle
     */
    bool allMinionHaveFinished();

    /**
     * Get the number of idle minions
     */
    unsigned char getNbIdleThreads();

    /**
     * Update the number of idle minions
     */
    unsigned char updateNbIdleMinion();

    /**
     * Lock the result list to interact with it
     */
    void resultListLock();

    /**
     * Unlock the result list
     */
    void resultListUnlock();

    /**
     * Display a debug message
     */
    void log(std::string msg);

    /**
     * Lock the log mutex
     */
    void logLock();
    /**
     * Unlock the log mutex
     */
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
