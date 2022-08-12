#include "../ECF_base.h"
#include "Tree.h"
#include "TreeCrxContextPreserved.h"
#include <stack>


namespace Tree
{

void TreeCrxContextPreserved::registerParameters(StateP state)
{
	myGenotype_->registerParameter(state, "crx.context", (voidP) new double(0), ECF::DOUBLE);
}


bool TreeCrxContextPreserved::initialize(StateP state)
{
	voidP sptr = myGenotype_->getParameterValue(state, "crx.context");
	probability_ = *((double*)sptr.get());
	return true;
}


bool TreeCrxContextPreserved::mate(GenotypeP gen1, GenotypeP gen2, GenotypeP ch)
{
	Tree* male = (Tree*) (gen1.get());
	Tree* female = (Tree*) (gen2.get());
	Tree* child = (Tree*) (ch.get());
		
	uint mRange, fRange;
	uint mIndex, fIndex;
	mRange = (uint) male->size();
	fRange = (uint) female->size();

	// create two vectors mapping the nodes with the same position in both trees (will also be at the same place in vector)
	std::vector <uint> maleCommonIndexes;
	std::vector <uint> femaleCommonIndexes;

	// skip nodes that do not have context-dependant pair
	std::stack <uint> maleSkip;
	std::stack <uint> femaleSkip;

	for(uint iMale = 0, iFemale = 0; iMale < mRange && iFemale < fRange; iMale++, iFemale++) {							
		// skip nodes in male parent
		while (!maleSkip.empty() && iMale == maleSkip.top()) {	
			maleSkip.pop();
			iMale += male->at(iMale)->size_;
		}		
		if (iMale >= mRange) break;
		
		// skip nodes in female parent
		while (!femaleSkip.empty() && iFemale == femaleSkip.top()) {	
			femaleSkip.pop();
			iFemale += female->at(iFemale)->size_;
		}
		if (iFemale >= fRange) break;

		// denote context-paired nodes
		maleCommonIndexes.push_back(iMale);
		femaleCommonIndexes.push_back(iFemale);
		
		uint maleArgNum = male->at(iMale)->primitive_->getNumberOfArguments();
		uint femaleArgNum = female->at(iFemale)->primitive_->getNumberOfArguments();
		
		if (maleArgNum > femaleArgNum) {

			std::vector<uint> toSkip;
			
			uint tmpIndex = iMale + 1;
			for(uint i = 0; i < maleArgNum; i++) {
				if (i >=  femaleArgNum) toSkip.push_back(tmpIndex);
				tmpIndex += male->at(tmpIndex)->size_;				
			}
			// cvorove cija podstabla treba preskociti stavljamo na stog tako da je uvijek slijedeci na vrhu
			for(int i = (int) toSkip.size() - 1; i >= 0; i--) 
				maleSkip.push(toSkip[i]);
		}
		if (maleArgNum < femaleArgNum) {

			std::vector<uint> toSkip;
			
			uint tmpIndex = iFemale + 1;
			for(uint i = 0; i < femaleArgNum; i++) {
				if (i >=  maleArgNum) toSkip.push_back(tmpIndex);
				tmpIndex += female->at(tmpIndex)->size_;				
			}
			// cvorove cija podstabla treba preskociti stavljamo na stog tako da je uvijek slijedeci na vrhu
			for(int i = (int) toSkip.size() - 1; i >= 0; i--) 
				femaleSkip.push(toSkip[i]);
		}
	}
	
	if(maleCommonIndexes.empty()) {
		ECF_LOG(state_, 5, "TreeCrxContextPreserved not successful.");
		return false;
	}

	uint rndIndex = state_->getRandomizer()->getRandomInteger((uint)maleCommonIndexes.size());

	mIndex = maleCommonIndexes[ rndIndex ];
	fIndex = femaleCommonIndexes[ rndIndex ];

	child->clear();

	// copy from male parent
	child->maxDepth_= male->maxDepth_;
	child->minDepth_ = male->minDepth_;
	child->startDepth_ = male->startDepth_;		

	// copy from male parent
	for(uint i = 0; i < mIndex; i++) {
		NodeP node = static_cast<NodeP> (new Node(male->at(i)->primitive_));
		child->push_back(node);
		child->at(i)->depth_ = male->at(i)->depth_;
	}
	// copy from female parent
	for(uint i = 0; i < female->at( fIndex )->size_; i++) {
		NodeP node = static_cast<NodeP> (new Node(female->at( fIndex + i)->primitive_));
		child->push_back(node);
	}
	// copy rest from male parent
	for(uint i = mIndex + male->at( mIndex )->size_; i < mRange; i++) {
		NodeP node = static_cast<NodeP> (new Node( male->at(i)->primitive_));
		child->push_back(node);
	}

	// update node depths and subtree sizes
	child->update();

	return true;
}

}