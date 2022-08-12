#ifndef SelRandomOp_h
#define SelRandomOp_h

#include "SelectionOperator.h"

/**
 * \ingroup evol selection
 * \brief Random individual selection operator.
 */

class SelRandomOp : public SelectionOperator
{
protected:
	RandomizerP randomizer_;
public:
	bool initialize(StateP);
	IndividualP select(const std::vector<IndividualP>&);
};
typedef boost::shared_ptr<SelRandomOp> SelRandomOpP;

#endif // SelRandomOp_h

