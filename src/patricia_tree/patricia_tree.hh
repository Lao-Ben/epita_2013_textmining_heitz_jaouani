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

    // Load the tree from a textual list of words
    void loadFromList(std::istream& input);

    // Load the tree from a serialized dictonary
    void loadFromDico(std::istream& input);

    // Serialize the tree
    void saveToDico(std::ostream& output);

    // Search a word in the tree
    void search(const char* word,
		unsigned int distance,
		std::list<SearchResult> collector);

  private:
    // Insert a new word
    void insert(const char* word);

    // Root of the tree
    PatriciaTreeNode* root_;

    // Keys storage
    std::vector<char> strings_;
};

#endif /* !PATRICIA_TREE_HH_ */
