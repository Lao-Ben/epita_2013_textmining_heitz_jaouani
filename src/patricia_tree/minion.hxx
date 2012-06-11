inline void
Minion::reInitKey(std::string& key)
{
  key.copy(key_, key.size());
  calculateDistance(0, key.size(), NULL, NULL);
}
