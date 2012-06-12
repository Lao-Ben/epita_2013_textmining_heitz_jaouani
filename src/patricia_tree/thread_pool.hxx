inline unsigned char
ThreadPool::getNbIdleThreads()
{
  return nbIdleThreads_;
}


inline void
ThreadPool::resultListLock()
{
  pthread_mutex_lock(&resultListMutex_);
}

inline void
ThreadPool::resultListUnlock()
{
  pthread_mutex_unlock(&resultListMutex_);
}


inline void
ThreadPool::logLock()
{
  pthread_mutex_lock(&logMutex_);
}

inline void
ThreadPool::logUnlock()
{
  pthread_mutex_unlock(&logMutex_);
}


inline bool
ThreadPool::wannaQuit()
{
  return wannaQuit_;
}


