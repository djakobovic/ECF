#include "../ECF_base.h"
#include "Tree.h"
#include "TreeCrxSimple.h"


void TreeCrxSimple::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.simple", (voidP) new double(0), DOUBLE);
}


bool TreeCrxSimple::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.simple");
	probability_ = *((double*)sptr.get());
	return true;
}


// TODO: reimplement for efficiency
bool TreeCrxSimple::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP ch)
{
	Tree* male = (Tree*) (gen1.get());
	Tree* female = (Tree*) (gen2.get());
	Tree* child = (Tree*) (ch.get());

	uint mIndex, fIndex;
	uint mRange, fRange;
	uint mNodeDepth, fNodeDepth, fNodeDepthSize;
	mRange = (uint) male->size();
	fRange = (uint) female->size();

	// check depth limitations
	uint nTries = 0;
	while(1) {
		mIndex = state_->getRandomizer()->getRandomInteger(0 , mRange -1 );
		fIndex = state_->getRandomizer()->getRandomInteger(0 , fRange -1 );
		mNodeDepth = male->at( mIndex )->depth_;
		fNodeDepth = female->at( fIndex )->depth_;

		// find max depth
		int maxDepth = fNodeDepth, depth;
		for(uint i = 0; i < female->at( fIndex )->size_; i++) {
			depth = female->at( fIndex + i )->depth_;
			maxDepth = depth > maxDepth ? depth : maxDepth;
		}

		fNodeDepthSize = maxDepth - fNodeDepth;
		nTries++;

		if( nTries > 4 || mNodeDepth + fNodeDepthSize <= male->maxDepth_ ) break;
	}

	if(nTries > 4 && mNodeDepth + fNodeDepthSize > male->maxDepth_) {
		state_->getLogger()->log(5, "TreeCrxSimple not successful.");
		return false;
	}

	// empty the child tree
	child->clear();

	// copy from male parent
	child->maxDepth_ = male->maxDepth_;
	child->minDepth_ = male->minDepth_;
	child->startDepth_ = male->startDepth_;

	// copy from male parent
	for(uint i = 0; i < mIndex; i++) {
		NodeP node = static_cast<NodeP> (new Node( male->at(i)->primitive_));
		child->push_back( node );
		child->at( i )->depth_ = male->at( i )->depth_;
	}
	// copy from female parent
	for(uint i = 0; i < female->at( fIndex )->size_; i++) {
		NodeP node = static_cast<NodeP> (new Node( female->at( fIndex + i)->primitive_));
		child->push_back( node );
	}
	// copy rest from male parent
	for(uint i = mIndex + male->at( mIndex )->size_; i < mRange; i++) {
		NodeP node = static_cast<NodeP> (new Node( male->at( i )->primitive_));
		child->push_back( node );
	}

	// update node depths and subtree sizes
	child->update();

	return true;
}
