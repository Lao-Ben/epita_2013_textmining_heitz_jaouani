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
PatriciaTree::insert(const char* word,
		     unsigned int frequency)
{
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


const char*
PatriciaTree::getData()
{
  return strings_.data();
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

typedef struct searchInfos
{
    PatriciaTreeNode& node;
    ThreadPool& pool;
} s_searchInfos;


// void* launch(void* argInfos)
// {
//   s_searchInfos* infos = (s_searchInfos*)argInfos;
//   pthread_cond_t treeParsed = PTHREAD_COND_INITIALIZER;
//   pthread_mutex_t treeParsedMutex;
//   pthread_mutex_init(&treeParsedMutex, NULL);
//   std::string prefix("");
//   infos->node.search(infos->pool, prefix, &treeParsed);
//   pthread_cond_wait(&treeParsed, &treeParsedMutex);
//   return NULL;
// }

// void
// PatriciaTree::search(ThreadPool& pool)
// {
//   struct searchInfos infos = {root_, pool};
//   //std::cout << "Tree: parsing start !" << std::endl;
//   pthread_t thread;
//   pthread_create(&thread, NULL, launch, &infos);
//   pthread_join(thread, NULL);

//   //std::cout << "Tree: parsing done !" << std::endl;
// }


void
PatriciaTree::search(ThreadPool& pool)
{
  std::string prefix("");
  root_.search(pool, prefix);
}

