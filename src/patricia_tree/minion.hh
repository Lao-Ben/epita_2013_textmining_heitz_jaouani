#ifndef MINION_HH_
# define MINION_HH_

# include <cstring>
# include <list>
# include <iostream>


# include "search_result.hh"
# include "patricia_tree_node_app.hh"

class ThreadPool;

class Minion
{
  public:
    Minion(ThreadPool* pool, unsigned char num);
    ~Minion();

    void run();
    void configure(const char* word,
		   unsigned char maxDistance,
		   const char* treeData,
		   std::list<SearchResult>* collector);
    bool isIdle();


  private:
    bool getATask();
    void browseNode(PatriciaTreeNodeApp* node, unsigned char keyLen);
    void browseNode0(PatriciaTreeNodeApp* node, unsigned char keyLen);
    void tableDisplay(std::ostream& out, unsigned char keyLen);
    void calculateDistance(unsigned char oldKeyLen,
			   unsigned char keyLen,
			   unsigned char* minDistance,
			   unsigned char* realDistance);

    bool log(std::string msg);


    pthread_mutex_t configure_;

    ThreadPool* pool_;

    const char* word_;

    const char* treeData_;

    char* key_;

    unsigned char** cmpTable_;

    std::list<SearchResult>* collector_;

    unsigned char keyBufferSize_;
    unsigned char wordLen_;
    unsigned char maxDistance_;
    unsigned char cmpTableSize_;
    unsigned char cmpTableActualSize_;
    unsigned char num_;
    bool isIdle_;
    char alignPadding_[1];
};

# include "minion.hxx"

#endif /* !MINIONS_HH_ */
