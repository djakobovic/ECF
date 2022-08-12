#ifndef Random_Search_h
#define Random_Search_h

#include "Algorithm.h"

/**
 * \brief Random search algorithm
 * \ingroup algorithms serial
 *
 * The algorithm flow:
 *
 \verbatim
  single generation {
   reinitialize all individuals;
   evaluate all individuals;
  }
 \endverbatim  
 */
class RandomSearch : public Algorithm
{
public:
	RandomSearch();
	bool advanceGeneration(StateP state, DemeP deme);
	bool initialize(StateP state);
	void registerParameters(StateP state);
};
typedef boost::shared_ptr<RandomSearch> RandomSearchP;

#endif // Random_Search_h