#ifndef MINION_HH_
# define MINION_HH_

# include <cstring>
# include <list>
# include <iostream>


# include "search_result.hh"
# include "patricia_tree_node.hh"

class ThreadPool;

class Minion
{
  public:
    Minion(ThreadPool* pool, int num);
    ~Minion();

    void run();
    void configure(const char* word,
		   unsigned int maxDistance,
		   const char* treeData,
		   std::list<SearchResult>& collector);

  private:
    bool getATask();
    void deleteTable();
    void browseNode(PatriciaTreeNode* node, unsigned char keyLen);
    void browseNode0(PatriciaTreeNode* node, unsigned char keyLen);
    void reInitKey(std::string& key);
    void tableDisplay(std::ostream& out, size_t keyLen);
    void calculateDistance(unsigned char oldKeyLen,
			   unsigned char keyLen,
			   unsigned char* minDistance,
			   unsigned char* realDistance);

    bool log(std::string msg);

    int num_;
    ThreadPool* pool_;
    const char* word_;
    const char* treeData_;
    char* key_;
    size_t wordLen_;
    size_t maxDistance_;
    std::list<SearchResult>* collector_;
    size_t cmpTableSize_;
    unsigned char** cmpTable_;
};

# include "minion.hxx"

#endif /* !MINIONS_HH_ */