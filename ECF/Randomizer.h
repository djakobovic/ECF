#ifndef Randomizer_h
#define Randomizer_h

class State;
typedef boost::shared_ptr<State> StateP;

/**
 * \ingroup evol main
 * \brief Abstract Randomizer class
 */
class Randomizer 
{
public:
	Randomizer() {}

	virtual ~Randomizer() {}

	/// Returns random integer in [p, q]
	virtual int getRandomInteger(int p, int q) = 0;

	/// Returns radnom integer in [0, p - 1]
	virtual int getRandomInteger(uint p) = 0;

	/// Returns random double
	virtual double getRandomDouble() = 0;

	virtual void registerParameters(StateP s) {}

	virtual bool initialize(StateP s) 
	{	return true;	}
};

typedef boost::shared_ptr<Randomizer> RandomizerP;

#endif

