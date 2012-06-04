#ifndef PATRICIA_TREE_HH_
# define PATRICIA_TREE_HH_

# include <vector>
# include <iostream>
# include "patricia_tree_node.hh"
# include "search_result.hh"


class PatriciaTree
{
  public:
    PatriciaTree();
    ~PatriciaTree();

    // Insert a new word
    void insert(const char* word,
		unsigned int frequency);

    // Load the tree from a serialized dictonary
    void loadFromDico(std::istream& input);

    // Serialize the tree
    void saveToDico(std::ostream& output);

    // get raw data
    const char* getData();

    // Search a word in the tree
    void search(ThreadPool& pool);

    void display(std::ostream& out);

  private:
    // Root of the tree
    PatriciaTreeNode root_;

    // Keys storage
    std::string strings_;
};

#endif /* !PATRICIA_TREE_HH_ */
