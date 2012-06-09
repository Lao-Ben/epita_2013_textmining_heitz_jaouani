#include "patricia_tree_node.hh"
#include "thread_pool.hh"

PatriciaTreeNode::PatriciaTreeNode()
  : start_ (0),
    length_ (0),
    frequency_ (0)
{
}

PatriciaTreeNode::PatriciaTreeNode(unsigned int start,
				   unsigned int length,
				   unsigned int frequency)
  : start_ (start),
    length_ (length),
    frequency_ (frequency)
{

}


PatriciaTreeNode::~PatriciaTreeNode()
{
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
  {
    delete *it;
  }
}


PatriciaTreeNode* addSon(std::list<PatriciaTreeNode*>& sons,
			 std::string& data,
			 unsigned int dataLen,
			 const char* word,
			 unsigned int wordLen,
			 unsigned int frequency)
{
  PatriciaTreeNode* node = new PatriciaTreeNode(dataLen,
						wordLen,
						frequency);
  if (strlen(word) == 0)
  {
    std::cout << "STRLEN=0"<< std::endl;
  }
  sons.push_back(node);
  data.append(word);
  return node;
}

std::string
PatriciaTreeNode::getStr(const char* data)
{
  std::string str(data + start_, length_);
  return str;
}


void
PatriciaTreeNode::insert(const char* word,
			 unsigned int wordLen,
			 unsigned int frequency,
			 std::string& data,
			 unsigned int dataLen)
{
  //std::cout << "INSERT" << std::endl;
  std::list<PatriciaTreeNode*>::iterator it;
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
      size_t pos;
      //std::cout << "wordlen: " << wordLen
      //<< "\tkeylen: " << keyLen << std::endl;
      for (pos = 0;
	   pos < wordLen // While the word is long enough
	     &&
	     pos < keyLen // While the key is long enough
	     &&
	     word[pos] == data[(*it)->start_ + pos]; // While the chars are equal
	   pos++)
      {
	// Nothing to do here !
	//std::cout << word[pos] << " - " << data[(*it)->start_ + pos] << std::endl;
      }

      // The word already exists
      //std::cout << "pos: " << pos << std::endl;
      if (pos == keyLen && pos == wordLen)
      {
	if ((*it)->frequency_ > 0)
	{
	  // std::cout << "Word already exists: ";
	  // std::cout << getStr(data.data())
	  // 	    << "  freq: " << (*it)->frequency_ << std::endl;
	  // std::cout << "trying to add        " << word
	  // 	    << " freq: " << frequency << std::endl;
	}
	(*it)->frequency_ = frequency;
	return;
      }

      if (pos >= keyLen) // The word is equal but longer than the key
      {
	// We insert in his son
	//std::cout << "Going deeper..." << std::endl;
	(*it)->insert(word + keyLen, wordLen - keyLen, frequency, data, dataLen);
	return;
      }

      // We have to split the current node
      PatriciaTreeNode* newNode =
	new PatriciaTreeNode((*it)->start_ + pos,
			     keyLen - pos,
			     (*it)->frequency_);
      (*it)->length_ = pos;
      newNode->sons_ = (*it)->sons_;
      (*it)->frequency_ = frequency;
      (*it)->sons_.clear();
      (*it)->sons_.push_back(newNode);
      //std::cout << "Node Split" << std::endl;

      if (pos < wordLen)
      {
	// There has been a difference between the two
	(*it)->frequency_ = 0;
	// std::cout << dataLen << std::endl;
	// std::cout << wordLen << std::endl;
	// std::cout << keyLen << std::endl;
	// std::cout << dataLen + wordLen - keyLen << std::endl;
	addSon((*it)->sons_, data, dataLen,
	       word + pos, wordLen - pos, frequency);
	//std::cout << "2nd son added" << std::endl;
      }
      return;
    }
  }
  // The first letter has not been found,
  // We create a new son
  //std::cout << "New Node" << std::endl;
  addSon(sons_, data, dataLen, word, wordLen, frequency);
}


size_t
PatriciaTreeNode::getStrStart()
{
  return start_;
}



size_t
PatriciaTreeNode::getStrLength()
{
  return length_;
}

size_t
PatriciaTreeNode::getFrequency()
{
  return frequency_;
}


std::list<PatriciaTreeNode*>&
PatriciaTreeNode::getSons()
{
  return sons_;
}




void
PatriciaTreeNode::display(std::ostream& out,
			  std::string& data,
			  std::string prefix)
{
  out << prefix << "[";
  out.write(data.data() + start_, length_) << "]";
  if (frequency_ > 0)
    out << " -";
  //int size = sons_.size();
  //if (size > 0)
  //out << " - " << size << " sons";
  out << std::endl;
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
    (*it)->display(out, data, prefix + "   ");
}


void
PatriciaTreeNode::serialize(std::ostream& out, std::string& data)
{
  out.write((char*)&frequency_, sizeof(frequency_));
  //if (frequency_ > 0)
  //std::cout << "*" << std::endl;
  unsigned char nbSon = sons_.size();
  out.write((char*)&nbSon, sizeof(nbSon));
  out.write(data.data() + start_, length_);
  out << '\0';
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
    (*it)->serialize(out, data);
}

void
PatriciaTreeNode::unserialize(std::istream& in, std::string& data)
{
  start_ = data.size();
  in.read((char*)&frequency_, sizeof(frequency_));
  //if (frequency_ > 0)
  //std::cout << "*" << std::endl;
  unsigned char nbSon = 0;
  in.read((char*)&nbSon, sizeof(nbSon));

  length_ = 0;
  char c;
  while('\0' != (c = in.peek()) && !in.eof() && !in.fail() && !in.bad())
  {
    data += c;
    in.ignore();
    length_++;
  }
  in.ignore();

  for (
    ;
    nbSon > 0;
    nbSon--
    )
  {
    PatriciaTreeNode* newNode = new PatriciaTreeNode();
    newNode->unserialize(in, data);
    sons_.push_back(newNode);
  }
}



void
PatriciaTreeNode::search(ThreadPool& pool,
			 std::string& prefix)
{
  for (
    std::list<PatriciaTreeNode*>::iterator it = sons_.begin();
    it != sons_.end();
    it++
    )
  {
//    std::cout <<"submit\n";
    pool.submitTask(*it, prefix);
  }
}
