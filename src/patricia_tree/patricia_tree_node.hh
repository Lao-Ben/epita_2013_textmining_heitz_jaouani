#ifndef PATRICIA_TREE_NODE_HH_
# define PATRICIA_TREE_NODE_HH_

# include <list>
# include <string>
# include <iostream>

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

    void display(std::ostream& out, std::string& data, std::string prefix);

    void serialize(std::ostream& out);
    void unserialize(std::istream& in);

  private:
    std::list<PatriciaTreeNode*> sons_;
    unsigned int start_;
    unsigned char length_;
    unsigned int frequency_;
};

std::ostream& operator<<(std::ostream& out, PatriciaTreeNode& node);


#endif /* !PATRICIA_TREE_NODE_HH_ */
