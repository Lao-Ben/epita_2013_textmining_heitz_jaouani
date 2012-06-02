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
PatriciaTree::saveToDico(std::ostream& output)
{
  root_.serialize(output);
  output << strings_;
}

unsigned int
PatriciaTree::loadFromDico(std::istream& input)
{
  unsigned int nbEntries = 0;
  root_.unserialize(input, nbEntries);
  getline(input, strings_);
  return nbEntries;
}


void
PatriciaTree::search(const char* word,
		     unsigned int maxDistance,
		     std::list<SearchResult>& collector)
{
  //FIXME
}

