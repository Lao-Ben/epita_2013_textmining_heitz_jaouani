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
  return (first.getKey().compare(second.getKey()) < 0);
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
	<< "\",\"freq\":"
	<< it->getFrequency()
	<< ",\"distance\":"
	<< it->getDistance()
	<< "}";
    if (size-- > 1)
      out << ",";
  }
  out << "]" << std::endl;
}
