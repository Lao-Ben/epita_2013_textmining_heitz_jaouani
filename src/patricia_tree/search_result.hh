#ifndef SEARCH_RESULT_HH_
# define SEARCH_RESULT_HH_

# include <string>

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
    const std::string& getKey();
    unsigned int getFrequency();
    unsigned int getDistance();

  private:
    std::string key_;
    unsigned int frequency_;
    unsigned int distance_;
};

#endif /* !SEARCH_RESULT_HH_ */
