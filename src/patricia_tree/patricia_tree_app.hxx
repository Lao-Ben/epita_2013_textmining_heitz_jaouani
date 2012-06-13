#include <cstring>


inline const char*
PatriciaTreeApp::getData()
{
  return strings_.data();
}


inline void
PatriciaTreeApp::loadFromDico(std::istream& input)
{
  root_.unserialize(input);
  input >> strings_;
}


inline void
PatriciaTreeApp::search(ThreadPool& pool)
{
  std::string prefix("");
  root_.search(pool, prefix);
}

