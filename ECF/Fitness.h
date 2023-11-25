#ifndef Fitness_h
#define Fitness_h

class Individual;
typedef std::shared_ptr<Individual> IndividualP;
class Fitness;
typedef std::shared_ptr<Fitness> FitnessP;

/**
 * \ingroup evol main
 * \brief Fitness base class
 *
 * A fitness object is used to represent individual's appropriateness for the problem.
 */
class Fitness
{
protected:
	bool valid_;	//!< is fitness value up-to-date
	double value_;	//!< actual fitness value

	// regular pointer (http://www.boost.org/doc/libs/1_36_0/libs/smart_ptr/sp_techniques.html#from_this)
	// initialized in Individual::initialize()
	Individual* individual_;

public:
	uint cid;	//!< coherence index, used in asynchronous parallel algoritmhs

	Fitness()
	{	
		valid_ = true;
		cid = 0;
	}

	/**
	 * \brief Mandatory comparison operator.
	 * \return true if this object is 'better' than the given parameter fitness object
	 */
	virtual bool isBetterThan(FitnessP) = 0;

	virtual Fitness* copy() = 0;	//!< object copy (for duplication of an individual)

	/// Is fitness object up to date.
	bool isValid()
	{	return valid_;	}

	/// Set fitness object to invalid state (called after any change of individual's genotype).
	void setInvalid()
	{	valid_ = false;	}

	/// Set fitness value.
	virtual void setValue(double value)
	{	value_ = value;	}

	/// Return fitness value.
	virtual double getValue()
	{	return value_;	}

	/// Write fitness object to XMLNode.
	virtual void write(XMLNode&) { }

	/// Output fitness XML format as std::string.
	std::string toString()
	{
		XMLNode xFit;
		write(xFit);
		char *s = xFit.createXMLString();
		std::string out(s);
		freeXMLString(s);
		return out;
	}

	/**
	 * \brief Read single fitness value from XML node.
	 * If the value is undefined (e.g. when reading unevaluated individual from XMLNode), validity is set to false.
	 */
	virtual void read(XMLNode& xFitness)
	{
		XMLCSTR val = xFitness.getAttributeValue();
		if(val) {
			value_ = xmltof(val);
			valid_ = true;
		}
		else
			valid_ = false;
	}

	virtual ~Fitness()
	{	}
};
typedef std::shared_ptr<Fitness> FitnessP;

#endif // Fitness_h

