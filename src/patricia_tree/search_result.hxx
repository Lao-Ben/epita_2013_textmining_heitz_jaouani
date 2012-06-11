inline void
SearchResult::setKey(const std::string& key)
{
  key_ = key;
}



inline void
SearchResult::setFrequency(unsigned int freq)
{
  frequency_ = freq;
}


inline void
SearchResult::setDistance(unsigned int distance)
{
  distance_ = distance;
}



inline const std::string&
SearchResult::getKey() const
{
  return key_;
}



inline unsigned int
SearchResult::getFrequency() const
{
  return frequency_;
}



inline unsigned int
SearchResult::getDistance() const
{
  return distance_;
}


