#ifndef SelectionOperator_h
#define SelectionOperator_h

#include "State.h"
#include "Individual.h"


/** \defgroup selection Selection Operators
 *  \ingroup evol
 */

/**
 * \ingroup evol selection
 * \brief Selection operator base class.
 *
 * All selection operators select one individual from a set, using different methods.
 */
class SelectionOperator
{
protected:
	StateP state_;
public:
	virtual ~SelectionOperator()
	{	}

	/**
	 * \brief Selection operator initialization.
	 * Must be called before individual selection.
	 */
	virtual bool initialize(StateP) = 0;

	/// Select one individual from a set.
	virtual IndividualP select(const std::vector<IndividualP>&) = 0;

};
typedef boost::shared_ptr<SelectionOperator> SelectionOperatorP;

#endif // SelectionOperator_h

