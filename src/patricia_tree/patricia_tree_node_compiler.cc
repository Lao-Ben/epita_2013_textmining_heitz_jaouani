#include "patricia_tree_node_compiler.hh"
#include "thread_pool.hh"


PatriciaTreeNodeCompiler::PatriciaTreeNodeCompiler()
{
}


PatriciaTreeNodeCompiler::PatriciaTreeNodeCompiler(unsigned int start,
						   unsigned int length,
						   unsigned int frequency)
  : AbstractPatriciaTreeNode(start, length, frequency)
{
}



PatriciaTreeNodeCompiler::~PatriciaTreeNodeCompiler()
{
  for (
    std::list<PatriciaTreeNodeCompiler*>::iterator it = sons_.begin();
    it != sons_.end();
    it++
    )
  {
    delete *it;
  }
}





PatriciaTreeNodeCompiler* addSon(std::list<PatriciaTreeNodeCompiler*>& sons,
				 std::string& data,
				 unsigned int dataLen,
				 const char* word,
				 unsigned int wordLen,
				 unsigned int frequency)
{
  PatriciaTreeNodeCompiler* node = new PatriciaTreeNodeCompiler(dataLen,
								wordLen,
								frequency);
  sons.push_back(node);
  data.append(word);
  return node;
}


void
PatriciaTreeNodeCompiler::insert(const char* word,
			 unsigned int wordLen,
			 unsigned int frequency,
			 std::string& data,
			 unsigned int dataLen)
{
  //std::cout << "INSERT" << std::endl;
  std::list<PatriciaTreeNodeCompiler*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
  {
    if (data[(*it)->start_] == word[0]) // The first letter is found
    {
      // We wanna know how far the keys are equal
      unsigned int keyLen = (*it)->length_;
      unsigned int pos;
      for (pos = 0;
	   pos < wordLen // While the word is long enough
	     &&
	     pos < keyLen // While the key is long enough
	     &&
	     word[pos] == data[(*it)->start_ + pos]; // While the chars are equal
	   pos++)
      {
      }

      // The word already exists
      if (pos == keyLen && pos == wordLen)
      {
	(*it)->frequency_ = frequency;
	return;
      }

      if (pos >= keyLen) // The word is equal but longer than the key
      {
	// We insert in his son
	(*it)->insert(word + keyLen, wordLen - keyLen, frequency, data, dataLen);
	return;
      }

      // We have to split the current node
      PatriciaTreeNodeCompiler* newNode =
	new PatriciaTreeNodeCompiler((*it)->start_ + pos,
			     keyLen - pos,
			     (*it)->frequency_);
      (*it)->length_ = pos;
      newNode->sons_ = (*it)->sons_;
      (*it)->frequency_ = frequency;
      (*it)->sons_.clear();
      (*it)->sons_.push_back(newNode);

      if (pos < wordLen)
      {
	// There has been a difference between the two
	(*it)->frequency_ = 0;
	addSon((*it)->sons_, data, dataLen,
	       word + pos, wordLen - pos, frequency);
      }
      return;
    }
  }
  // The first letter has not been found,
  // We create a new son
  addSon(sons_, data, dataLen, word, wordLen, frequency);
}



void
PatriciaTreeNodeCompiler::serialize(std::ostream& out)
{
  out.write((char*)&start_, sizeof(start_));
  out.write((char*)&length_, sizeof(length_));
  out.write((char*)&frequency_, sizeof(frequency_));
  unsigned char nbSon = sons_.size();
  out.write((char*)&nbSon, sizeof(nbSon));
  std::list<PatriciaTreeNodeCompiler*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
    (*it)->serialize(out);
}
