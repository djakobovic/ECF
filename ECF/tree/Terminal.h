#ifndef Terminal_h
#define Terminal_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Terminal tree node class (Tree genotype).
 *
 * Terminal ia s Primitive that represents a single value (double by default).
 */
template <class T>
class TerminalT : public Primitive
{
public:
	TerminalT();
	T value_;
	void execute(void* result, Tree& tree);
	void setValue(void* value);
	void getValue(void* value);
	~TerminalT();
};
// default type is double:
typedef TerminalT<double> Terminal;


template <class T>
TerminalT<T>::TerminalT()
{
	nArguments_ = 0;
}


template <class T>
TerminalT<T>::~TerminalT(void)
{	}


/**
 * \brief	Set terminal's value.
 * \param	value pointer to new value
 */
template <class T>
void TerminalT<T>::setValue(void* value)
{
	value_ = *(T*)value;
}


/**
 * \brief	Get terminal's value.
 * \param	pointer to destination
 */
template <class T>
void TerminalT<T>::getValue(void* value)
{
	*(T*)value = value_;
}


template <class T>
void TerminalT<T>::execute(void* result, Tree& tree)
{
	T& res = *(T*)result;
	res = value_;
}



/**
 * \ingroup tree genotypes primitives
 * \brief Ephemereal random constant (ERC) node class (Tree genotype).
 *
 * ERC is a Primitive that represents a single value (of type double by default) 
 * that can assume values from the interval or from the set. 
 * Value of an ERC instance is defined once - when assigning to a node in a new tree. 
 * Only double-typed ERC instances may change value (with Gaussian mutation). 
 *
 * ERC's are distinguished from regular terminals by the prefix denoting their type, e.g.:
 * D_3.14, I_5, B_true, C_a
 */
template <class T>
class ERC : public Primitive
{
public:
	std::string baseName_;
	T value_;

	ERC()
	{
		nArguments_ = 0;
		name_ = "ERC";
		// ime se treba postaviti u Tree::initialize zbog prefiksa po tipu
	}


	/**
	 * \brief	Initialize ERC value(s).
	 *
	 * Read given values from config and store in vector.
	 */
	bool initialize(StateP state)
	{
		state_ = state;
		values_ = new std::vector<T>;

		name_.erase(name_.end() - 1);
		std::stringstream ss;
		ss << name_;
		ss >> name_;
		char bracket;
		ss >> bracket;

		T datum;
		while(ss >> datum) {
			values_->push_back(datum);
		}

		return true;
	}


	void execute(void *result, Tree& tree)
	{
		T& res = *(T*)result;
		res = value_;
	}


	/**
	 * \brief	Set terminal's value.
	 * \param	value pointer to new value
	 */
	void setValue(void* value)
	{
		value_ = *(T*)value;
	}


	/**
	 * \brief	Get terminal's value.
	 * \param	pointer to destination
	 */
	void getValue(void* value)
	{
		*(T*)value = value_;
	}


	PrimitiveP copyWithNode(PrimitiveP primitive)
	{
		return (PrimitiveP) new ERC<T>(*this);
	}

	/**
	 * \brief	Create (or choose) new unique ERC value and assign to tree node.
	 * \param	pointer to destination
	 */
	PrimitiveP assignToNode(PrimitiveP primitive)
	{
		// shallow copy - the values_ vector is not copied
		ERC<T> *erc = new ERC<T>(*this);

		uint iData = state_->getRandomizer()->getRandomInteger((int)values_->size());
		erc->value_ = values_->at(iData);
		std::ostringstream ss;
		ss << values_->at(iData);
		erc->setName(name_ + ss.str());
		return (PrimitiveP) erc;
	}

protected:
	/// Placeholder for predefined values that this type of ERC may assume. 
	/// Each ERC instance holds only a single value.
	std::vector<T> *values_;
};



/**
 * \ingroup tree genotypes primitives
 * \brief Ephemereal random constant (ERC) node of type double (Tree genotype).
 *
 * Possible values may be defined in the configuration with the interval [x y] or with a set {a b c ...}
 */
class ERCD : public ERC<double>
{
public:
	bool initialize(StateP state)
	{
		useInterval_ = true;
		state_ = state;
		values_ = new std::vector<double>;

		if(name_[name_.size() - 1] == '}')
			useInterval_ = false;
		name_.erase(name_.end() - 1);
		std::stringstream ss;
		ss << name_;
		ss >> name_;
		char bracket;
		ss >> bracket;

		double datum;
		while(ss >> datum) {
			values_->push_back(datum);
		}

		return true;
	}

	PrimitiveP assignToNode(PrimitiveP primitive)
	{
		// shallow copy - the values_ are not copied
		ERCD *erc = new ERCD(*this);

		// generate a random value in the interval
		if(useInterval_) {
			double r = state_->getRandomizer()->getRandomDouble();
			erc->value_ = (*values_)[0] + r * ((*values_)[1] - (*values_)[0]);
		}
		// or choose a random predefined value
		else {
			uint iData = state_->getRandomizer()->getRandomInteger((int)values_->size());
			erc->value_ = values_->at(iData);
		}
		std::ostringstream ss;
		ss << erc->value_;
		erc->setName(name_ + ss.str());
		return (PrimitiveP) erc;
	}

protected:
		bool useInterval_;    /// are we assuming values in the interval (or from a set)
};

}
}

#endif
