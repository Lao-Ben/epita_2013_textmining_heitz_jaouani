#include "patricia_tree_node.hh"


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
  sons.push_back(node);
  data.append(word);
  return node;
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
	(*it)->frequency_ = frequency;
	//std::cout << "Word already exists" << std::endl;
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
  out << prefix
//      << start_ << " " << end_ << "  " << data.size()
      << "[ \"";
  out.write(data.data() + start_, length_);
  out << "\"\t"
      << start_
      << " - "
      << (int)length_
      << "\t"
      << frequency_
      << " ]";
  int size = sons_.size();
  if (size > 0)
    out << " - " << size << " sons";
  out << std::endl;
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
    (*it)->display(out, data, prefix + "\t");
}


void
PatriciaTreeNode::serialize(std::ostream& out)
{
  out.write((char*)&start_, sizeof(start_));
  out.write((char*)&length_, sizeof(length_));
  out.write((char*)&frequency_, sizeof(frequency_));
  unsigned char nbSon = sons_.size();
  out.write((char*)&nbSon, sizeof(nbSon));
  std::list<PatriciaTreeNode*>::iterator it;
  for (
    it = sons_.begin();
    it != sons_.end();
    it++
    )
    (*it)->serialize(out);
}

void
PatriciaTreeNode::unserialize(std::istream& in)
{
  in.read((char*)&start_, sizeof(start_));
  in.read((char*)&length_, sizeof(length_));
  in.read((char*)&frequency_, sizeof(frequency_));
  unsigned char nbSon;
  in.read((char*)&nbSon, sizeof(nbSon));
  // std::cout << "===" << std::endl;
  // std::cout << start_ << std::endl;
  // std::cout << (int)length_ << std::endl;
  // std::cout << frequency_ << std::endl;
  // std::cout << (int)nbSon << std::endl;
  for (
    ;
    nbSon > 0;
    nbSon--
    )
  {
    PatriciaTreeNode* newNode = new PatriciaTreeNode();
    newNode->unserialize(in);
    sons_.push_back(newNode);
  }
}
