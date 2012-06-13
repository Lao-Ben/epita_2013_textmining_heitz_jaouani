#include "patricia_tree_node_app.hh"
#include "thread_pool.hh"



PatriciaTreeNodeApp::PatriciaTreeNodeApp()
{
}


PatriciaTreeNodeApp::PatriciaTreeNodeApp(unsigned int start,
					 unsigned int length,
					 unsigned int frequency)
  : AbstractPatriciaTreeNode(start, length, frequency)
{
}




PatriciaTreeNodeApp::~PatriciaTreeNodeApp()
{
  for (
    std::vector<PatriciaTreeNodeApp*>::iterator it = sons_.begin();
    it != sons_.end();
    it++
    )
  {
    delete *it;
  }
}



void
PatriciaTreeNodeApp::unserialize(std::istream& in)
{
  in.read((char*)&start_, sizeof(start_));
  in.read((char*)&length_, sizeof(length_));
  in.read((char*)&frequency_, sizeof(frequency_));
  unsigned char nbSon = 0;
  in.read((char*)&nbSon, sizeof(nbSon));
  sons_.resize(nbSon);
  for (unsigned char i = 0; i < nbSon; i++)
  {
    PatriciaTreeNodeApp* newNode = new PatriciaTreeNodeApp();
    newNode->unserialize(in);
    sons_[i] = newNode;
  }
}




void
PatriciaTreeNodeApp::search(ThreadPool& pool,
			 std::string& prefix)
{
  for (
    std::vector<PatriciaTreeNodeApp*>::iterator it = sons_.begin();
    it != sons_.end();
    it++
    )
    pool.submitTask(*it, prefix);
}
