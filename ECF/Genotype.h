#ifndef Genotype_h
#define Genotype_h

#include<string>

// forward declarations of genotype operators
class CrossoverOp;
typedef boost::shared_ptr<CrossoverOp> CrossoverOpP;
class MutationOp;
typedef boost::shared_ptr<MutationOp> MutationOpP;

// forward declaration of State class
class State;
typedef boost::shared_ptr<State> StateP;

/**
 * \ingroup evol population
 * \ingroup genotypes
 * \brief Genotype base class
 *
 * All Genotypes must inherit this one. 
 * Each individual contains a vector of Genotype objects. Each Genotype is an independent object with independent parameters.
 *
 * Each Genotype class must provide its CrossoverOp and MutationOp operator classes!
 *
 * \par Genotype usage
 * A Genotype instance is _active_ if it is used in the individual structure. 
 * Active genotypes may be of any number and kind; each active Genotype instance gets an index, e.g.:
 * - Binary (id: 0) Binary (id: 1) Bitstring (id: 2) Tree (id: 3)
 *
 * A genotype instance becomes active if:
 * - it is stated in <Genotype> block of configuration file;
 * - it is added with State::setGenotype function.
 */
class Genotype
{
protected:

public:
	Genotype()
	{	}
	virtual ~Genotype()
	{	}

	/// Initialize a genotype object (read parameters, perform sanity check, build data)
	virtual bool initialize(StateP state) = 0;

	/// Create an identical copy of the genotype object
	virtual Genotype* copy() = 0;

	/// Create and return a vector of crossover operators
	virtual std::vector<CrossoverOpP> getCrossoverOp()
	{
		std::vector<CrossoverOpP> empty;
		return empty;
	}

	/// Create and return a vector of mutation operators
	virtual std::vector<MutationOpP> getMutationOp()
	{
		std::vector<MutationOpP> empty;
		return empty;
	}

	/// Register genotype's parameters (called before Genotype::initialize)
	virtual void registerParameters(StateP)	{}

	bool registerParameter(StateP state, std::string name, voidP value, enum ECF::type T, std::string description = ""); //!< Register a single parameter

	voidP getParameterValue(StateP state, std::string name);	//!< Read single parameter value from Registry

	bool setParameterValue(StateP state, std::string name, voidP value);	//!< Write single parameter value to Registry

	bool isParameterDefined(StateP state, std::string name);	//!< Check if parameter is defined in the configuration

	/// Read genotype data from XMLNode 
	virtual void read(XMLNode&)	= 0;

	/// Write genotype data to XMLNode 
	virtual void write(XMLNode&) = 0;

	virtual uint getGenomeSize()
	{	return 0;	}

	/// Return genotype's name (each genotype is uniquely identified with its name).
	std::string getName()
	{	return name_;	}

	/// Return this genotype's index in individual structure
	uint getGenotypeId()
	{	return genotypeId_;	}

	/// Set genotype index in an individual
	void setGenotypeId(uint id)
	{	genotypeId_ = id;	}

	/// Output genotype to string
	std::string toString()
	{
		XMLNode xGene;
		write(xGene);
		char *s = xGene.createXMLString();
		std::string out(s);
		freeXMLString(s);
		return out;
	}

protected:
	std::string name_;	//!< genotype's name
	uint genotypeId_;	//!< this genotype's unique index in individual structure
};
typedef boost::shared_ptr<Genotype>  GenotypeP;

#endif // Genotype_h

