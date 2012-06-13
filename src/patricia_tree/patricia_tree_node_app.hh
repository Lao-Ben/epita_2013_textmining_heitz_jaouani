#ifndef PATRICIA_TREE_NODE_APP_HH_
# define PATRICIA_TREE_NODE_APP_HH_

# include <vector>
# include <string>
# include <iostream>

# include "abstract_patricia_tree_node.hh"

class ThreadPool;

class PatriciaTreeNodeApp : public AbstractPatriciaTreeNode
{
  public:
    PatriciaTreeNodeApp();

    PatriciaTreeNodeApp(unsigned int start,
			unsigned int end,
			unsigned int frequency);

    ~PatriciaTreeNodeApp();

    std::vector<PatriciaTreeNodeApp*>& getSons();

    void unserialize(std::istream& in);

    void search(ThreadPool& pool,
		std::string& prefix);

    std::vector<PatriciaTreeNodeApp*> sons_;
};

# include "patricia_tree_node_app.hxx"


#endif /* !PATRICIA_TREE_NODE_APP_HH_ */
