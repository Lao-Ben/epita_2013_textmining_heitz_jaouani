#ifndef PATRICIA_TREE_COMPILER_HH_
# define PATRICIA_TREE_COMPILER_HH_

# include "abstract_patricia_tree.hh"
# include "patricia_tree_node_compiler.hh"


class PatriciaTreeCompiler : public AbstractPatriciaTree
{
  public:
    PatriciaTreeCompiler();
    ~PatriciaTreeCompiler();
    // Insert a new word
    void insert(const char* word,
		unsigned int frequency);

    // Serialize the tree
    void saveToDico(std::ostream& output);

  private:
    // Root of the tree
    PatriciaTreeNodeCompiler root_;
};

# include "patricia_tree_compiler.hxx"

#endif /* !PATRICIA_TREE_COMPILER_HH_ */
