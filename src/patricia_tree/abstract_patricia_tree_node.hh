#ifndef ABSTRACT_PATRICIA_TREE_NODE_HH_
# define ABSTRACT_PATRICIA_TREE_NODE_HH_


/**
 * Abstract version of patricia trie, containing
 * the common features of the Compiler and App version
 */

class AbstractPatriciaTreeNode
{
  public:

    /**
     * Default constructor
     */
    AbstractPatriciaTreeNode();


    /**
     * Constructor with fields setting
     */
    AbstractPatriciaTreeNode(unsigned int start,
			     unsigned int length,
			     unsigned int frequency);


    /**
     * Get the ley's start address
     */
    unsigned int getStrStart();

    /**
     * Get the key's length
     */
    unsigned char getStrLength();

    /**
     * Get the key's frequency
     */
    unsigned int getFrequency();

  protected:
    unsigned int start_;
    unsigned int frequency_;
    unsigned char length_;
    char alignPadding_[7];
};

# include "abstract_patricia_tree_node.hxx"


#endif /* !ABSTRACT_PATRICIA_TREE_NODE_HH_ */
