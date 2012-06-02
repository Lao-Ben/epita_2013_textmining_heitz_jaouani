#include "search_result.hh"



SearchResult::SearchResult()
  : frequency_ (0),
    distance_ (0)
{

}


SearchResult::SearchResult(std::string& key,
			   unsigned int distance,
			   unsigned int freq)
  :key_ (key),
   frequency_ (freq),
   distance_ (distance)
{
  
}


SearchResult::~SearchResult()
{

}


void
SearchResult::setKey(const std::string& key)
{
  key_ = key;
}



void
SearchResult::setFrequency(unsigned int freq)
{
  frequency_ = freq;
}


void
SearchResult::setDistance(unsigned int distance)
{
  distance_ = distance;
}



const std::string&
SearchResult::getKey()
{
  return key_;
}



unsigned int
SearchResult::getFrequency()
{
  return frequency_;
}



unsigned int
SearchResult::getDistance()
{
  return distance_;
}
