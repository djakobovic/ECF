#include "../ECF_base.h"
#include "Tree.h"
#include "TreeCrxUniform.h"


namespace Tree
{

void TreeCrxUniform::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.uniform", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeCrxUniform::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.uniform");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeCrxUniform::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP ch)
{
	Tree* male = (Tree*) (gen1.get());
	Tree* female = (Tree*) (gen2.get());
	Tree* child = (Tree*) (ch.get());

	uint mRange, fRange;

	mRange = (uint) male->size();
	fRange = (uint) female->size();

	// copy from male parent
	child->clear();
	child->maxDepth_ = male->maxDepth_;
	child->minDepth_ = male->minDepth_;
	child->startDepth_ = male->startDepth_;

	for( uint iMale = 0, iFemale = 0, iChild=0; iMale < mRange && iFemale < fRange; ) {

		// choose either male or female node from the common region
		uint pickMale = state_->getRandomizer()->getRandomInteger(0 , 1);

		if( male->at( iMale )->primitive_->getNumberOfArguments() == female->at( iFemale )->primitive_->getNumberOfArguments() ) {
			// if the number of arguments is the same, choose randomly from either parent
			if( pickMale ) {
				NodeP node = static_cast<NodeP> (new Node( male->at( iMale )->primitive_));
				child->push_back( node );
				child->at( iChild )->depth_ = male->at( iMale )->depth_;
			}
			else {
				NodeP node = static_cast<NodeP> (new Node( female->at( iFemale )->primitive_));
				child->push_back( node );
				child->at( iChild )->depth_ = female->at( iFemale )->depth_;
			}		
			iMale++; iFemale++; iChild++;
		}
		else {
			// if numbers of arguments differ, copy subtree to child
			if(pickMale) {
				for( uint i = 0; i < male->at( iMale )->size_; i++, iChild++ ) {
					NodeP node = static_cast<NodeP> (new Node( male->at( iMale + i )->primitive_));
					child->push_back( node );
					child->at( iChild )->depth_ = male->at( iMale + i )->depth_;
				}				
			}
			else {
				for( uint i = 0; i < female->at( iFemale )->size_; i++, iChild++ ) {
					NodeP node = static_cast<NodeP> (new Node( female->at( iFemale + i )->primitive_));
					child->push_back( node );
					child->at( iChild )->depth_ = female->at( iFemale + i )->depth_;
				}				
			}
			// skip nodes not in common region
			iMale += male->at( iMale )->size_;
			iFemale += female->at( iFemale )->size_;
		}
	}

	// update node depths and subtree sizes
	child->update();

	return true;
}

}