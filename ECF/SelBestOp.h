#ifndef SELBESTOP_H_
#define SELBESTOP_H_

#include "SelectionOperator.h"

/**
 * \ingroup evol selection
 * \brief Best individual selection operator 
 */
class SelBestOp: public SelectionOperator {
public:
	bool initialize(StateP);
	IndividualP select(const std::vector<IndividualP>&);
};

typedef boost::shared_ptr<SelBestOp> SelBestOpP;

#endif /* SELBESTOP_H_ */

