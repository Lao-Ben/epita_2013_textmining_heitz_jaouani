#ifndef ABSTRACT_PATRICIA_TREE_NODE_HH_
# define ABSTRACT_PATRICIA_TREE_NODE_HH_

class AbstractPatriciaTreeNode
{
  public:
    AbstractPatriciaTreeNode();

    AbstractPatriciaTreeNode(unsigned int start,
			     unsigned int length,
			     unsigned int frequency);

    unsigned int getStrStart();
    unsigned char getStrLength();
    unsigned int getFrequency();

  protected:
    unsigned int start_;
    unsigned int frequency_;
    unsigned char length_;
    char alignPadding_[7];
};

# include "abstract_patricia_tree_node.hxx"


#endif /* !ABSTRACT_PATRICIA_TREE_NODE_HH_ */
