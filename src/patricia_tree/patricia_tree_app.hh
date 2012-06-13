#ifndef PATRICIA_TREE_APP_HH_
# define PATRICIA_TREE_APP_HH_

# include "abstract_patricia_tree.hh"
# include "patricia_tree_node_app.hh"
# include "thread_pool.hh"


/**
 * Store the actual tree of the App version
 * Search functions are inplemented
 */
class PatriciaTreeApp : public AbstractPatriciaTree
{
  public:
    /**
     * Default Constructor
     */
    PatriciaTreeApp();

    /**
     * Default destructor
     */
    ~PatriciaTreeApp();

    /**
     * Load the tree from a serialized dictonnary
     */
    void loadFromDico(std::istream& input);

    /**
     * Get raw data
     */
    const char* getData();

    /**
     * Search a word in the tree
     */
    void search(ThreadPool& pool);

  private:
    // Root of the tree
    PatriciaTreeNodeApp root_;

};

# include "patricia_tree_app.hxx"

#endif /* !PATRICIA_TREE_APP_HH_ */
