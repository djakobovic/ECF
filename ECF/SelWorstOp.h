#ifndef SelWorstOp_h
#define SelWorstOp_h

#include "SelectionOperator.h"

/**
 * \ingroup evol selection
 * \brief Worst individual selection operator.
 */
class SelWorstOp : public SelectionOperator
{
public:
	bool initialize(StateP);
	IndividualP select(const std::vector<IndividualP>&);
};
typedef boost::shared_ptr<SelWorstOp> SelWorstOpP;

#endif // SelWorstOp_h

