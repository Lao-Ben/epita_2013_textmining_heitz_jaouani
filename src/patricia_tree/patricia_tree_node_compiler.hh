#ifndef PATRICIA_TREE_NODE_COMPILER_HH_
# define PATRICIA_TREE_NODE_COMPILER_HH_

# include <list>
# include <string>
# include <iostream>

# include "abstract_patricia_tree_node.hh"

/**
 * Compiler version of the patricia tree node, used to compile the dictionnary.
 * The sons are stored in a std::list.
 */

class PatriciaTreeNodeCompiler : public AbstractPatriciaTreeNode
{
  public:

    /**
     * Default constructor
     */
    PatriciaTreeNodeCompiler();

    /**
     * Construction with field setting
     */
    PatriciaTreeNodeCompiler(unsigned int start,
			     unsigned int end,
			     unsigned int frequency);


    /**
     * Default destructor. Recursivly destroy sons
     */
    ~PatriciaTreeNodeCompiler();


    /**
     * Recursivly insert a new key in the tree
     */
    void insert(const char* word,
		unsigned int wordLen,
		unsigned int frequency,
		std::string& data,
		unsigned int dataLen);


    /**
     * Serialize a compact binary version of the tree
     */
    void serialize(std::ostream& out);


    /**
     * Store the son nodes
     */
    std::list<PatriciaTreeNodeCompiler*> sons_;
};


#endif /* !PATRICIA_TREE_NODE_COMPILER_HH_ */
