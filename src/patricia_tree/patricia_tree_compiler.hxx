#include <cstring>

inline void
PatriciaTreeCompiler::insert(const char* word,
		     unsigned int frequency)
{
  root_.insert(word, strlen(word), frequency, strings_, strings_.size());

}


inline void
PatriciaTreeCompiler::saveToDico(std::ostream& output)
{
  root_.serialize(output);
  output << strings_;
}

