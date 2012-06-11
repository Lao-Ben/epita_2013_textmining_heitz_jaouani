#include <cstring>
#include <iostream>
#include <pthread.h>
#include "patricia_tree.hh"
#include "thread_pool.hh"

PatriciaTree::PatriciaTree()
{

}


PatriciaTree::~PatriciaTree()
{

}



void
PatriciaTree::display(std::ostream& out)
{
  out << "==================================" << std::endl;
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = root_.getSons().begin();
    it != root_.getSons().end();
    it++
    )
  {
    (*it)->display(std::cout, strings_, "");
  }
  // out << "==================================" << std::endl;
  // out << "============ D A T A =============" << std::endl;
  // out << "==================================" << std::endl;
  // out << strings_ << std::endl;
  // out << "==================================" << std::endl;
}
