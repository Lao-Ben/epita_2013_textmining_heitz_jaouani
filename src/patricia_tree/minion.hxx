


inline bool
Minion::isIdle()
{
  return isIdle_;
}




inline unsigned char min2(unsigned char a, unsigned char b)
{
  if (a > b)
    return b;
  else
    return a;
}

inline unsigned char min3(unsigned char a, unsigned char b, unsigned char c)
{
  return min2(a, min2(b, c));
}

