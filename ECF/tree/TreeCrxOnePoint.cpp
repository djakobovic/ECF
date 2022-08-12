#include "../ECF_base.h"
#include "Tree.h"
#include "TreeCrxOnePoint.h"


namespace Tree
{

void TreeCrxOnePoint::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.onepoint", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeCrxOnePoint::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.onepoint");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeCrxOnePoint::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP ch)
{
	Tree* male = (Tree*) (gen1.get());
	Tree* female = (Tree*) (gen2.get());
	Tree* child = (Tree*) (ch.get());

	uint mIndex, fIndex;
	uint mRange, fRange;
	uint mNodeDepth, fNodeDepth, fNodeDepthSize;

	mRange = (uint) male->size();
	fRange = (uint) female->size();

	// for common region nodes
	std::vector <uint> maleCommonRegionIndexes;	
	std::vector <uint> femaleCommonRegionIndexes;	

	for( uint iMale = 0, iFemale = 0; iMale < mRange && iFemale < fRange; iMale++, iFemale++ ) {
		// add common region nodes
		maleCommonRegionIndexes.push_back( iMale );
		femaleCommonRegionIndexes.push_back( iFemale );

		// skip nodes with different no. of arguments
		if( male->at( iMale )->primitive_->getNumberOfArguments() != female->at( iFemale )->primitive_->getNumberOfArguments() ) {
			iMale += male->at( iMale )->size_;
			iFemale += female->at( iFemale )->size_;
		}
	}

	uint nTries = 0;
	while(1) {
		// choose random node from common region
		uint randomNode = state_->getRandomizer()->getRandomInteger(0 , (int) maleCommonRegionIndexes.size()-1 );
		mIndex = maleCommonRegionIndexes[ randomNode ];
		fIndex = femaleCommonRegionIndexes[ randomNode ];

		// LD: provjera dubine je redundantna u slucaju da sve jedinke imaju jednaki maxDepth_
		// ili kada otac ima veci ili jednak maxDepth_

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
		ECF_LOG(state_, 5, "TreeCrxOnePoint not successful.");
		return false;
	}

	child->clear();
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