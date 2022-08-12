#ifndef Operator_h
#define Operator_h

/**
 * \ingroup evol main
 * \brief Abstract operator class.
 *
 * Encapsulates calls to parameter registration, initialization and operation.
 */
class Operator
{
public:
	virtual bool operate(StateP) = 0;	//!< perform the designated operation

	/**
	 * \brief Perform initialization. Called before Operator::operate.
	 * By default, if the return value is false, the operator will not be used!
	 * \return	initialization success
	 */
	virtual bool initialize(StateP)	
	{	return true;	}

	/**
	 * \brief Register parameters with the Registry.
	 * Called before Operator::initialize.
	 */
	virtual void registerParameters(StateP)
	{	}

	/**
	 * \brief Write operator state to XMLNode or the Registry.
	 * Called after Operator::initialize.
	 */
	virtual void write(XMLNode&)
	{	}

	/**
	 * \brief Read operator state from XMLNode or the Registry.
	 * Called after Operator::initialize.
	 */
	virtual void read(XMLNode&)
	{	}

	virtual ~Operator()
	{	}
};
typedef boost::shared_ptr<Operator> OperatorP;

#endif // Operator_h
