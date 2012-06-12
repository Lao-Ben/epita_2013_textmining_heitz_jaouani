#ifndef PATRICIA_TREE_NODE_HH_
# define PATRICIA_TREE_NODE_HH_

# include <list>
# include <string>
# include <iostream>

# include "search_result.hh"

class ThreadPool;


class PatriciaTreeNode
{
  public:
    PatriciaTreeNode();
    PatriciaTreeNode(unsigned int start,
		     unsigned int end,
		     unsigned int frequency);
    ~PatriciaTreeNode();

    void insert(const char* word,
		unsigned int wordLen,
		unsigned int frequency,
		std::string& data,
		unsigned int dataLen);

    std::list<PatriciaTreeNode*>& getSons();

    size_t getStrStart();
    size_t getStrLength();
    size_t getFrequency();

    void display(std::ostream& out, std::string& data, std::string prefix);

    void serialize(std::ostream& out, std::string& data);
    void unserialize(std::istream& in, std::string& data);

    void search(ThreadPool& pool,
		std::string& prefix);

    std::list<PatriciaTreeNode*> sons_;

  private:
    unsigned int start_;
    unsigned char length_;
    unsigned int frequency_;
};

std::ostream& operator<<(std::ostream& out, PatriciaTreeNode& node);

# include "patricia_tree_node.hxx"

#endif /* !PATRICIA_TREE_NODE_HH_ */
