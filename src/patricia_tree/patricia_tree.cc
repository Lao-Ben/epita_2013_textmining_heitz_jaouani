#include "patricia_tree.hh"
#include <string.h>
#include <iostream>

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
//  std::cout << "WORD \"" << word << "\"" << std::endl;
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
  out << "==================================" << std::endl;
  out << "============ D A T A =============" << std::endl;
  out << "==================================" << std::endl;
  out << strings_ << std::endl;
  out << "==================================" << std::endl;
}


void
PatriciaTree::search(const char* word,
		     unsigned int distance,
		     std::list<SearchResult>& collector)
{

}


void
PatriciaTree::saveToDico(std::ostream& output)
{
  root_.serialize(output);
  output << strings_;
}

void
PatriciaTree::loadFromDico(std::istream& input)
{
  root_.unserialize(input);
  getline(input, strings_);
}

