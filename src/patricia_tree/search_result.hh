#ifndef SEARCH_RESULT_HH_
# define SEARCH_RESULT_HH_

# include <string>
# include <list>
# include <iostream>

/**
 * Store a result when fetched by a minion
 */

class SearchResult
{
  public:

    /**
     * Default constructor
     */
    SearchResult();

    /**
     * Constructor with the fields set
     */
    SearchResult(std::string& key,
		 unsigned int distance,
		 unsigned int freq);

    /**
     * Default destructor
     */
    ~SearchResult();

    // Setters

    /**
     * Set the found key
     */
    void setKey(const std::string& key);

    /**
     * Set the frequency of the found key
     */
    void setFrequency(unsigned int freq);

    /**
     * Set the distance between the word and the key
     */
    void setDistance(unsigned int distance);

    // Getters

    /**
     * Get the key
     */
    const std::string& getKey() const;

    /**
     * Get the frequency of the key
     */
    unsigned int getFrequency() const;

    /**
     * Get the distance between the word and the key
     */
    unsigned int getDistance() const;

  private:
    std::string key_;
    unsigned int frequency_;
    unsigned int distance_;
};


/**
 * Ordering fonction used to sort the results list
 */
bool resultCompare(const SearchResult& first, const SearchResult& second);

/**
 * Print the list on JSon format
 */
void exportJSon(std::list<SearchResult>& resultCollector, std::ostream& out);

# include "search_result.hxx"

#endif /* !SEARCH_RESULT_HH_ */
