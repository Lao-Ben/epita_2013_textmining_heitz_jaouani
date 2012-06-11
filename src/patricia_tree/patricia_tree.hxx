#include <cstring>

inline void
PatriciaTree::insert(const char* word,
		     unsigned int frequency)
{
  root_.insert(word, strlen(word), frequency, strings_, strings_.size());

}


inline const char*
PatriciaTree::getData()
{
  return strings_.data();
}

inline void
PatriciaTree::saveToDico(std::ostream& output)
{
  root_.serialize(output, strings_);
}

inline void
PatriciaTree::loadFromDico(std::istream& input)
{
  root_.unserialize(input, strings_);
}

inline void
PatriciaTree::search(ThreadPool& pool)
{
  std::string prefix("");
  root_.search(pool, prefix);
}

