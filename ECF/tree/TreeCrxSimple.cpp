#include "../ECF_base.h"
#include "Tree.h"
#include "TreeCrxSimple.h"


namespace Tree
{

void TreeCrxSimple::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.simple", (voidP) new double(0), ECF::DOUBLE);
	myGenotype_->registerParameter(state, "crx.simple.functionprob", (voidP) new double(0.9), ECF::DOUBLE);
}


bool TreeCrxSimple::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.simple");
	probability_ = *((double*)sptr.get());
	sptr = myGenotype_->getParameterValue(state, "crx.simple.functionprob");
	funcChoiceProb_ = *((double*)sptr.get());

	return true;
}


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

	// indentify leaf (terminal) and nonLeaf (function) nodes
	std::vector <uint> maleLeafIndexes, maleNonLeafIndexes;	
	for(uint i = 0; i < mRange; i++) {
		if( male->at( i )->size_ == 1 ) maleLeafIndexes.push_back( i );
		else maleNonLeafIndexes.push_back( i );
	}
	// analogno za female
	std::vector <uint> femaleLeafIndexes, femaleNonLeafIndexes;
	for(uint i = 0; i < fRange; i++) {
		if( female->at( i )->size_ == 1 ) femaleLeafIndexes.push_back( i );
		else femaleNonLeafIndexes.push_back( i );
	}

	uint nTries = 0;
	while(1) {
		// choose male crx point
		if(state_->getRandomizer()->getRandomDouble() > funcChoiceProb_ || maleNonLeafIndexes.empty()) {
			uint randomLeafIndex = state_->getRandomizer()->getRandomInteger(0 , (int) maleLeafIndexes.size() - 1);
			mIndex = maleLeafIndexes [ randomLeafIndex ];
		}
		else {
			uint randomNonLeafIndex = state_->getRandomizer()->getRandomInteger(0 , (int) maleNonLeafIndexes.size() - 1);
			mIndex = maleNonLeafIndexes [ randomNonLeafIndex ];
		}

		// choose female crx point
		if(state_->getRandomizer()->getRandomDouble() > funcChoiceProb_ || femaleNonLeafIndexes.empty()) {
			uint randomLeafIndex = state_->getRandomizer()->getRandomInteger(0 , (int) femaleLeafIndexes.size() - 1);
			fIndex = femaleLeafIndexes [ randomLeafIndex ];
		}
		else {
			uint randomNonLeafIndex = state_->getRandomizer()->getRandomInteger(0 , (int) femaleNonLeafIndexes.size() - 1);
			fIndex = femaleNonLeafIndexes [ randomNonLeafIndex ];
		}

		mNodeDepth = male->at( mIndex )->depth_;
		fNodeDepth = female->at( fIndex )->depth_;

		// find max depth
		uint maxDepth = fNodeDepth, depth;
		for(uint i = 0; i < female->at( fIndex )->size_; i++) {
			depth = female->at( fIndex + i )->depth_;
			maxDepth = depth > maxDepth ? depth : maxDepth;
		}

		fNodeDepthSize = maxDepth - fNodeDepth;
		nTries++;

		if(nTries > 4 || mNodeDepth + fNodeDepthSize <= male->maxDepth_ ) break;
	}

	if(nTries > 4 && mNodeDepth + fNodeDepthSize > male->maxDepth_) {
		ECF_LOG(state_, 5, "TreeCrxSimple not successful.");
		return false;
	}

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

}