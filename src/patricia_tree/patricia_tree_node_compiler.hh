#ifndef PATRICIA_TREE_NODE_COMPILER_HH_
# define PATRICIA_TREE_NODE_COMPILER_HH_

# include <list>
# include <string>
# include <iostream>

# include "abstract_patricia_tree_node.hh"


class PatriciaTreeNodeCompiler : public AbstractPatriciaTreeNode
{
  public:
    PatriciaTreeNodeCompiler();
    PatriciaTreeNodeCompiler(unsigned int start,
			     unsigned int end,
			     unsigned int frequency);

    ~PatriciaTreeNodeCompiler();

    void insert(const char* word,
		unsigned int wordLen,
		unsigned int frequency,
		std::string& data,
		unsigned int dataLen);

    void serialize(std::ostream& out);

    std::list<PatriciaTreeNodeCompiler*> sons_;
};


#endif /* !PATRICIA_TREE_NODE_COMPILER_HH_ */
