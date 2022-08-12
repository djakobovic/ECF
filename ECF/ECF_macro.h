#ifndef ECF_macro_h
#define ECF_macro_h

#include <sstream>

inline std::string uint2str(uint nNumber)
{
	std::ostringstream oss;
	oss << nNumber;
	return oss.str();
}


inline std::string dbl2str(double dNumber)
{
	std::ostringstream oss;
	oss << dNumber;
	return oss.str();
}


inline int str2int(const std::string& str)
{
  std::istringstream iss(str);
  int integer;
  iss >> integer;
  return integer;
}


inline uint str2uint(const std::string& str)
{
  std::istringstream iss(str);
  uint integer;
  iss >> integer;
  return integer;
}


inline double str2dbl(const std::string& str)
{
  std::istringstream iss(str);
  double d;
  iss >> d;
  return d;
}


inline float str2flt(const std::string& str)
{
  std::istringstream iss(str);
  float f;
  iss >> f;
  return f;
}


#endif // ECF_macro_h
