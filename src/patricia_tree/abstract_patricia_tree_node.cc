#include "abstract_patricia_tree_node.hh"

AbstractPatriciaTreeNode::AbstractPatriciaTreeNode()
{
}


AbstractPatriciaTreeNode::AbstractPatriciaTreeNode(unsigned int start,
						   unsigned int length,
						   unsigned int frequency)
  : start_ (start),
    frequency_ (frequency),
    length_ (length)
{
}


