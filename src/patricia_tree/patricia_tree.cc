#include <cstring>
#include <iostream>
#include "patricia_tree.hh"
#include "thread_pool.hh"

PatriciaTree::PatriciaTree()
{

}


PatriciaTree::~PatriciaTree()
{

}


void
PatriciaTree::insert(const char* word,
		     unsigned int frequency)
{
  //std::cout << "WORD \"" << word << "\"" << std::endl;
  root_.insert(word, strlen(word), frequency, strings_, strings_.size());
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



void
PatriciaTree::saveToDico(std::ostream& output)
{
  root_.serialize(output, strings_);
}

void
PatriciaTree::loadFromDico(std::istream& input)
{
  root_.unserialize(input, strings_);
}


void
PatriciaTree::search(ThreadPool& pool)
{
  root_.search(pool);
}

