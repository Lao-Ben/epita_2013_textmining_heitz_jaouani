#ifndef MINION_HH_
# define MINION_HH_

# include <cstring>
# include <list>
# include <iostream>


# include "search_result.hh"
# include "patricia_tree_node_app.hh"

class ThreadPool;

/**
 * The minions are the threads launched by the pool to perform the tasks in the
 * pool's todolist.
 */

class Minion
{
  public:

    /**
     * Create the minion with a number and a link to the pool
     */
    Minion(ThreadPool* pool, unsigned char num);

    /**
     * Destroy the minion
     */
    ~Minion();


    /**
     * Launch the reserching process
     */
    void run();

    /**
     * Configure the minion for a new research
     */
    void configure(const char* word,
		   unsigned char maxDistance,
		   const char* treeData,
		   std::list<SearchResult>* collector);

    /**
     * Return whether the minion does nothing
     */
    bool isIdle();


  private:

    /**
     * Try to get a new task from the pool's todolist.
     * Return true if succeded, false if the todolost is empty
     */
    bool getATask();

    /**
     * Browse a node and go deeper in its sons
     */
    void browseNode(PatriciaTreeNodeApp* node, unsigned char currentLen);

    /**
     * Quicker version of browseNode for distance=0
     */
    void browseNode0(PatriciaTreeNodeApp* node, unsigned char keyLen);


    /**
     * display the damerau-levenshtein distance calculation table (debug)
     */
    void tableDisplay(std::ostream& out, unsigned char keyLen);

    /**
     * Calculate the latest values on the distance table
     */
    void calculateDistance(unsigned char oldKeyLen,
			   unsigned char keyLen,
			   unsigned char* minDistance,
			   unsigned char* realDistance);


    /**
     * print a debug message
     */
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
