#ifndef ABSTRACT_PATRICIA_TREE_HH_
# define ABSTRACT_PATRICIA_TREE_HH_

#include <string>

/**
 * Abstract version of the patricia tree
 */
class AbstractPatriciaTree
{
  protected:

    /**
     * Default constructor
     */
    AbstractPatriciaTree();

    /**
     * Default destructor
     */
    ~AbstractPatriciaTree();

    // Keys storage
    std::string strings_;
};

#endif /* !ABSTRACT_PATRICIA_TREE_HH_ */
