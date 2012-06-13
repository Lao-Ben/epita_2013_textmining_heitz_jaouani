#ifndef PATRICIA_TREE_NODE_APP_HH_
# define PATRICIA_TREE_NODE_APP_HH_

# include <vector>
# include <string>
# include <iostream>

# include "abstract_patricia_tree_node.hh"

class ThreadPool;

/**
 * App version of the patricia trie, used on the TextMiningApp.
 * The sons are stored in a vector, in order to fasten the malloc
 * processing when unserializing
 */

class PatriciaTreeNodeApp : public AbstractPatriciaTreeNode
{
  public:

    /**
     * Default constructor
     */
    PatriciaTreeNodeApp();


    /**
     * Construction with fields setting
     */
    PatriciaTreeNodeApp(unsigned int start,
			unsigned int end,
			unsigned int frequency);


    /**
     * Default destructor. Recursivly destroy all the sons
     */
    ~PatriciaTreeNodeApp();


    /**
     * Get the son list
     */
    std::vector<PatriciaTreeNodeApp*>& getSons();


    /**
     * Build a tree from a stream
     */
    void unserialize(std::istream& in);


    /**
     * Submit all the sons on the pool todolist
     */
    void search(ThreadPool& pool,
		std::string& prefix);


    /**
     * Store the sons
     */
    std::vector<PatriciaTreeNodeApp*> sons_;
};

# include "patricia_tree_node_app.hxx"


#endif /* !PATRICIA_TREE_NODE_APP_HH_ */
