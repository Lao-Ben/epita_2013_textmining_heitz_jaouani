#ifndef ABSTRACT_PATRICIA_TREE_HH_
# define ABSTRACT_PATRICIA_TREE_HH_

#include <string>

class AbstractPatriciaTree
{
  protected:
    AbstractPatriciaTree();
    ~AbstractPatriciaTree();

    // Keys storage
    std::string strings_;
};

#endif /* !ABSTRACT_PATRICIA_TREE_HH_ */
