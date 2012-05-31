#ifndef PATRICIA_TREE_NODE_HH_
# define PATRICIA_TREE_NODE_HH_

# include <list>

class PatriciaTreeNode
{
  public:
    PatriciaTreeNode();
    ~PatriciaTreeNode();

  private:
    std::list<PatriciaTreeNode> sons_;
    unsigned int key_offset_;
    unsigned int key_length_;
};


#endif /* !PATRICIA_TREE_NODE_HH_ */
