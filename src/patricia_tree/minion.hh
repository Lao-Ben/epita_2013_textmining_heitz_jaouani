#ifndef MINION_HH_
# define MINION_HH_

# include <cstring>
# include <list>

# include "search_result.hh"


class ThreadPool;

class Minion
{
  public:
    Minion(ThreadPool& pool);
    ~Minion();

    void run();
    void configure(const char* word,
		   unsigned int maxDistance,
		   std::list<SearchResult>& collector);

  private:
    bool getATask();
    void deleteTable();

    ThreadPool& pool_;
    const char* word_;
    size_t wordLen_;
    size_t wordPos_;
    unsigned int maxDistance_;
    std::list<SearchResult>* collector_;
    size_t cmpTableSize_;
    char** cmpTable_;
};

#endif /* !MINIONS_HH_ */
