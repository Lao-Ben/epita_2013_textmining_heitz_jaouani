#ifndef PATRICIA_TREE_APP_HH_
# define PATRICIA_TREE_APP_HH_

# include "abstract_patricia_tree.hh"
# include "patricia_tree_node_app.hh"
# include "thread_pool.hh"

class PatriciaTreeApp : public AbstractPatriciaTree
{
  public:
    PatriciaTreeApp();
    ~PatriciaTreeApp();

    // Load the tree from a serialized dictonary
    void loadFromDico(std::istream& input);

    // get raw data
    const char* getData();

    // Search a word in the tree
    void search(ThreadPool& pool);

  private:
    // Root of the tree
    PatriciaTreeNodeApp root_;

};

# include "patricia_tree_app.hxx"

#endif /* !PATRICIA_TREE_APP_HH_ */
