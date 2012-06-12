inline void
Minion::reInitKey(std::string& key)
{
  key.copy(key_, key.size());
  calculateDistance(0, key.size(), NULL, NULL);
}

inline void
Minion::reInitKey0(std::string& key)
{
  key.copy(key_, key.size());
}



inline bool
Minion::isIdle()
{
  return isIdle_;
}
