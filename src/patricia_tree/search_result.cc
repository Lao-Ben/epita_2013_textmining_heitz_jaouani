#include "search_result.hh"
#include <cstring>


SearchResult::SearchResult()
  : frequency_ (0),
    distance_ (0)
{

}


SearchResult::SearchResult(std::string& key,
			   unsigned int distance,
			   unsigned int freq)
  : key_ (key),
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
SearchResult::getKey() const
{
  return key_;
}



unsigned int
SearchResult::getFrequency() const
{
  return frequency_;
}



unsigned int
SearchResult::getDistance() const
{
  return distance_;
}


bool resultCompare(const SearchResult& first, const SearchResult& second)
{
  if (first.getDistance() < second.getDistance())
    return true;
  if (first.getDistance() > second.getDistance())
    return false;

  // equal distance
  if (first.getFrequency() > second.getFrequency())
    return true;
  if (first.getFrequency() < second.getFrequency())
    return false;

  // equal frequency
  return strcmp(first.getKey().c_str(), second.getKey().c_str());
}


void exportJSon(std::list<SearchResult>& resultCollector, std::ostream& out)
{
  unsigned int size = resultCollector.size();
  out << "[";
  for (
    std::list<SearchResult>::iterator it = resultCollector.begin();
    it != resultCollector.end();
    it++
    )
  {
    out << "{\"word\":\""
	<< it->getKey()
	<< "\",\"freq\":\""
	<< it->getFrequency()
	<< "\",\"distance\":"
	<< it->getDistance()
	<< "}";
    if (size-- > 0)
      out << ",";
  }
  out << "]" << std::endl;
}
