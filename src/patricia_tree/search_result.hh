#ifndef SEARCH_RESULT_HH_
# define SEARCH_RESULT_HH_

# include <string>
# include <list>
# include <iostream>

class SearchResult
{
  public:
    SearchResult();
    SearchResult(std::string& key,
		 unsigned int distance,
		 unsigned int freq);
    ~SearchResult();

    // Setters
    void setKey(const std::string& key);
    void setFrequency(unsigned int freq);
    void setDistance(unsigned int distance);

    // Getters
    const std::string& getKey() const;
    unsigned int getFrequency() const;
    unsigned int getDistance() const;

  private:
    std::string key_;
    unsigned int frequency_;
    unsigned int distance_;
};

bool resultCompare(const SearchResult& first, const SearchResult& second);
void exportJSon(std::list<SearchResult>& resultCollector, std::ostream& out);

# include "search_result.hxx"

#endif /* !SEARCH_RESULT_HH_ */
