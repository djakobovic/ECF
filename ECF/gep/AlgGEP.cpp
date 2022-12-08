#include "AlgGEP.h"


AlgGEP::AlgGEP()
{
	name_ = "GEP";
	selFitPropOp = (SelFitnessProportionalOpP)(new SelFitnessProportionalOp);
	selBestOp = (SelBestOpP)(new SelBestOp);
	selRandomOp = (SelRandomOpP)(new SelRandomOp);
}

void AlgGEP::registerParameters(StateP state)
{
	registerParameter(state, "crxprob", (voidP) new double(0.5), ECF::DOUBLE, "crossover rate");
	registerParameter(state, "selpressure", (voidP) new double(10), ECF::DOUBLE,
		"selection pressure: how much is the best individual 'better' than the worst");
	registerParameter(state, "inv.prob", (voidP) new double(0.1), ECF::DOUBLE, "inversion rate");
	registerParameter(state, "erc.inv.prob", (voidP) new double(0.1), ECF::DOUBLE, "ERC inversion rate");
	registerParameter(state, "trans.is.prob", (voidP) new double(0.1), ECF::DOUBLE, "IS transposition rate");
	registerParameter(state, "trans.ris.prob", (voidP) new double(0.1), ECF::DOUBLE, "RIS transposition rate");
	registerParameter(state, "trans.gene.prob", (voidP) new double(0.1), ECF::DOUBLE, "gene transposition rate");
	registerParameter(state, "trans.erc.prob", (voidP) new double(0.1), ECF::DOUBLE, "ERC transposition rate");
	registerParameter(state, "trans.max.length", (voidP) new double(3), ECF::DOUBLE, "maximum length of the transposition sequence");
	registerParameter(state, "inv.max.length", (voidP) new double(3), ECF::DOUBLE, "maximum length of the inversion sequence");
}

bool AlgGEP::initialize(StateP state)
{
	selFitPropOp->initialize(state);
	selFitPropOp->setSelPressure(10);
	selBestOp->initialize(state);
	selRandomOp->initialize(state);

	voidP crRateP = getParameterValue(state, "crxprob");
	crxRate_ = *((double*)crRateP.get());

	voidP selPressP = getParameterValue(state, "selpressure");
	selPressure_ = *((double*)selPressP.get());
	selFitPropOp->setSelPressure(selPressure_);

	voidP invRateP = getParameterValue(state, "inv.prob");
	invRate_ = *((double*)invRateP.get());

	invRateP = getParameterValue(state, "erc.inv.prob");
	invDcRate_ = *((double*)invRateP.get());

	invRateP = getParameterValue(state, "inv.max.length");
	invMaxLength_ = *((double*)invRateP.get());

	voidP transRateP = getParameterValue(state, "trans.is.prob");
	transISRate_ = *((double*)transRateP.get());

	transRateP = getParameterValue(state, "trans.ris.prob");
	transRISRate_ = *((double*)transRateP.get());

	transRateP = getParameterValue(state, "trans.gene.prob");
	transGeneRate_ = *((double*)transRateP.get());

	transRateP = getParameterValue(state, "trans.erc.prob");
	transDcRate_ = *((double*)transRateP.get());

	transRateP = getParameterValue(state, "trans.max.length");
	transMaxLength_ = *((double*)transRateP.get());

	GEPChromosomeP gen(new GEP::GEPChromosome);
	if (state->getGenotypes()[0]->getName() != gen->getName()){
		ECF_LOG_ERROR(state, "Error: this algorithm accepts only a single GEPChromosome genotype.");
		throw("");
	}

	return true;
}

bool AlgGEP::advanceGeneration(StateP state, DemeP deme)
{
	// elitism: copy current best individual
	IndividualP best = selBestOp->select(*deme);
	best = copy(best);

	// select individuals
	std::vector<IndividualP> wheel;
	wheel = selFitPropOp->selectMany(*deme, (uint)deme->size());

	// copy selected to new population
	for (uint i = 0; i < wheel.size(); ++i)
		wheel[i] = copy(wheel[i]);

	// replace old population
	for (uint i = 0; i < deme->size(); i++)
		replaceWith((*deme)[i], wheel[i]);

	ECF_LOG(state, 5, "Selected individuals:");
	for (uint i = 0; i < deme->size(); i++){
		ECF_LOG(state, 5, dbl2str(deme->at(i)->fitness->getValue()));
	}

	// determine the number of crx operations
	uint noCrx = (int)(deme->size() * crxRate_ / 2);

	// perform crossover
	for (uint i = 0; i < noCrx; i++){

		// select parents
		IndividualP parent1 = selRandomOp->select(*deme);
		IndividualP parent2 = selRandomOp->select(*deme);
		ECF_LOG(state, 5, "Parents: " + dbl2str(parent1->fitness->getValue()) + ", " + dbl2str(parent2->fitness->getValue()));

		// create children
		IndividualP child1 = copy(parent1);
		IndividualP child2 = copy(parent2);

		// perform crx operations
		mate(parent1, parent2, child1);
		mate(parent1, parent2, child2);

		// replace parents with children
		replaceWith(parent1, child1);
		replaceWith(parent2, child2);
	}
	
	// perform mutation on whole population
	mutate(*deme);

	// perform inversion on population
	invert(state, *deme);
	invertDc(state, *deme);
	// perform transposition on population
	transpose(state, *deme);
	// evaluate new individuals
	for (uint i = 0; i < deme->size(); i++)
		if (!deme->at(i)->fitness->isValid()) {
			evaluate(deme->at(i));
		}

	// elitism: preserve best individual
	IndividualP random = selRandomOp->select(*deme);
	if (best->fitness->isBetterThan(random->fitness))
		replaceWith(random, best);

	return true;
}

void AlgGEP::invert(StateP state, const std::vector<IndividualP>& pool)
{
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= invRate_) {
			// invert the i-th individual
			ECF_LOG(state, 5, "Inverting individual: \n" + pool.at(i)->toString());
			// first, choose a random gene in the individual
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			pool.at(i)->fitness->setInvalid();
			uint invGene = state_->getRandomizer()->getRandomInteger(individual->genes);
			uint geneOffset = invGene * (individual->geneLength);

			// next, choose two random points in the head area
			uint bitInv = state_->getRandomizer()->getRandomInteger(individual->headLength);
			uint bitInvSecond, tmp;
			do
			{
				bitInvSecond = state_->getRandomizer()->getRandomInteger(individual->headLength);
			} while (bitInv == bitInvSecond);

			if (bitInv>bitInvSecond)
			{
				tmp = bitInv;
				bitInv = bitInvSecond;
				bitInvSecond = tmp;
			}
			// shorten the sequence to the specified maximum length
			if (bitInvSecond - bitInv >= invMaxLength_){
				bitInvSecond = bitInv +  (uint) invMaxLength_ - 1;
			}

			ECF_LOG(state, 5, "Inverting the sequence in gene ["+uint2str(invGene)+"] from point (" + uint2str(bitInv) + ") to point (" + uint2str(bitInvSecond) + ")");
			// copy the selected sequence in reverse order
			std::vector<Tree::NodeP> seq;
			for (int j = (geneOffset + bitInvSecond); j >= (int) (geneOffset + bitInv); j--){
				seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence with its inversion
			uint seqIdx = 0;
			for (uint j = (geneOffset + bitInv); j <= (geneOffset + bitInvSecond); j++){
				individual->at(j) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			ECF_LOG(state, 5, "Inverted individual: \n" + pool[i]->toString());
		}
			
	}
}

void AlgGEP::invertDc(StateP state, const std::vector<IndividualP>& pool)
{
	// Do a test and check whether the Dc domain actually exists
	GEPChromosomeP test = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(0)->getGenotype());
	if (test->dcLength < 1){
		ECF_LOG(state, 5, "ERCs not used in the cromosome. Dc inversion aborted.");
		return; // Do nothing if ERCs are not used
	}
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= invDcRate_) {
			// invert the i-th individual
			pool.at(i)->fitness->setInvalid();
			ECF_LOG(state, 5, "Inverting ERCs of individual: \n" + pool.at(i)->toString());
			// first, choose a random gene in the individual
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			uint invGene = state_->getRandomizer()->getRandomInteger(individual->genes);
			uint geneOffset = invGene * (individual->geneLength);
			uint dcStart = geneOffset + individual->headLength + individual->tailLength;

			// next, choose two random points in the Dc tail area
			uint bitInv = state_->getRandomizer()->getRandomInteger(individual->dcLength);
			uint bitInvSecond, tmp;
			do
			{
				bitInvSecond = state_->getRandomizer()->getRandomInteger(individual->dcLength);
			} while (bitInv == bitInvSecond);

			if (bitInv>bitInvSecond)
			{
				tmp = bitInv;
				bitInv = bitInvSecond;
				bitInvSecond = tmp;
			}
			// shorten the sequence to the specified maximum length
			if (bitInvSecond - bitInv >= invMaxLength_){
				bitInvSecond = bitInv + (uint)invMaxLength_ - 1;
			}

			ECF_LOG(state, 5, "Inverting the sequence in gene [" + uint2str(invGene) + "] from point (" + uint2str(bitInv) + ") to point (" + uint2str(bitInvSecond) + ")");
			// copy the selected sequence in reverse order
			std::vector<Tree::NodeP> seq;
			for (int j = (dcStart + bitInvSecond); j >= (int)(dcStart + bitInv); j--){
				seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence with its inversion
			uint seqIdx = 0;
			for (uint j = (dcStart + bitInv); j <= (dcStart + bitInvSecond); j++){
				individual->at(j) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			ECF_LOG(state, 5, "Inverted individual: \n" + pool[i]->toString());
		}

	}
}

void AlgGEP::transpose(StateP state, const std::vector<IndividualP>& pool)
{
	if(transISRate_ > 0) transposeIS(state, pool);
	if(transRISRate_ > 0) transposeRIS(state, pool);
	if(transGeneRate_ > 0) transposeGene(state, pool);
	if (transDcRate_ > 0) transposeDc(state, pool);

}

void AlgGEP::transposeIS(StateP state, const std::vector<IndividualP>& pool)
{
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= transISRate_) {
			ECF_LOG(state, 5, "Transposing individual using IS transposition: \n" + pool.at(i)->toString());
			pool.at(i)->fitness->setInvalid();
			// first, choose a random gene in the individual 
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			uint invGene = state_->getRandomizer()->getRandomInteger(individual->genes);
			uint geneOffset = invGene * (individual->geneLength);

			// next, choose two random points in the gene which delimit the copied sequence
			uint bitSeq = state_->getRandomizer()->getRandomInteger(individual->headLength+individual->tailLength);
			uint bitSeqSecond, tmp;
			do
			{
				bitSeqSecond = state_->getRandomizer()->getRandomInteger(individual->headLength+individual->tailLength);
			} while (bitSeq == bitSeqSecond);

			if (bitSeq > bitSeqSecond)
			{
				tmp = bitSeq;
				bitSeq = bitSeqSecond;
				bitSeqSecond = tmp;
			}

			// shorten the sequence to the specified maximum length
			if (bitSeqSecond - bitSeq >= transMaxLength_){
				bitSeqSecond = bitSeq + (uint) transMaxLength_ - 1;
			}


			// next, choose the random point in the head area in which the sequence will be inserted (root excluded)
			uint bitPos = state_->getRandomizer()->getRandomInteger(1, individual->headLength - 1);
			ECF_LOG(state, 5, "Transposing the sequence in gene [" + uint2str(invGene) + "] from point (" + uint2str(bitSeq) + ") to point (" + uint2str(bitSeqSecond) + ") into position (" + uint2str(bitPos) + ")");

			// make a copy of the entire gene
			std::vector<Tree::NodeP> seq;
			for (uint j = geneOffset; j < (geneOffset + individual->headLength + individual->tailLength); j++){
				seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence first with the transposon
			uint seqIdx = bitSeq;
			uint r;
			for (r = (geneOffset+bitPos); r <= (geneOffset+bitPos+bitSeqSecond-bitSeq) && r < (geneOffset+individual->headLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			// now, copy the original elements from the transposition point until the end of the head
			seqIdx = bitPos;
			for (; r < (geneOffset+individual->headLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			ECF_LOG(state, 5, "Transposed individual: \n" + pool[i]->toString());
		}
	}
}

void AlgGEP::transposeRIS(StateP state, const std::vector<IndividualP>& pool)
{
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= transRISRate_) {
			ECF_LOG(state, 5, "Transposing individual using RIS transposition: \n" + pool.at(i)->toString());
			// first, choose a random gene in the individual
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			uint invGene = state_->getRandomizer()->getRandomInteger(individual->genes);
			uint geneOffset = invGene * (individual->geneLength);

			// next, choose a random point in the head and scan until a function is found or the end of the head is reached
			uint bitSeq = state_->getRandomizer()->getRandomInteger(1, individual->headLength-1);
			while (bitSeq < individual->headLength && !(individual->primitiveSet_->getFunctionByName(individual->at(bitSeq)->primitive_->getName()))){
				bitSeq++;
			}
			// do nothing if no function found
			if (bitSeq >= individual->headLength){
				ECF_LOG(state, 5, "No function nodes found. Transposition aborted.");
				continue;
			}
			pool.at(i)->fitness->setInvalid();
			// randomly choose a point between the first one and the end of the gene
			uint bitSeqSecond, tmp;
			do
			{
				bitSeqSecond = state_->getRandomizer()->getRandomInteger(bitSeq,individual->headLength+individual->tailLength - 1);
			} while (bitSeq == bitSeqSecond);

			if (bitSeq > bitSeqSecond)
			{
				tmp = bitSeq;
				bitSeq = bitSeqSecond;
				bitSeqSecond = tmp;
			}

			// shorten the sequence to the specified maximum length
			if (bitSeqSecond - bitSeq >= transMaxLength_){
				bitSeqSecond = bitSeq + (uint) transMaxLength_ - 1;
			}

			// transpose to the root
			ECF_LOG(state, 5, "Transposing the sequence in gene [" + uint2str(invGene) + "] from point (" + uint2str(bitSeq) + ") to point (" + uint2str(bitSeqSecond) + ") into root");

			// make a copy of the entire gene
			std::vector<Tree::NodeP> seq;
			for (uint j = geneOffset; j < (geneOffset + individual->headLength + individual->tailLength); j++){
				seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence first with the transposon
			uint seqIdx = bitSeq;
			uint r;
			for (r = geneOffset; r <= (geneOffset + bitSeqSecond - bitSeq) && r < (geneOffset + individual->headLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			// now, copy the original elements from the transposition point until the end of the head
			seqIdx = 0;
			for (; r < (geneOffset + individual->headLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			ECF_LOG(state, 5, "Transposed individual: \n" + pool[i]->toString());
		}
	}
}

void AlgGEP::transposeGene(StateP state, const std::vector<IndividualP>& pool)
{
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= transGeneRate_) {
			ECF_LOG(state, 5, "Transposing individual using gene transposition: \n" + pool.at(i)->toString());
			// first, choose a random gene in the individual 
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			if (individual->genes == 1){
				ECF_LOG(state, 5, "Gene transposition failed: genotype consists of one gene");
				return;
			}
			pool.at(i)->fitness->setInvalid();
			uint invGene = state_->getRandomizer()->getRandomInteger(1,individual->genes-1);
			uint geneOffset = invGene * (individual->geneLength);

			ECF_LOG(state, 5, "Transposing gene (" + uint2str(invGene)+") to head");

			// make separate copies of the transposon gene and the rest of the chromosome
			std::vector<Tree::NodeP> seq, copy;
			for (uint j = 0; j < individual->size(); j++){
				if (j >= geneOffset && j < (geneOffset + individual->geneLength))
					seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
				else
					copy.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence first with the transposon
			uint copyIdx = 0;
			individual->clear();
			for (uint r = 0; r < seq.size(); r++){
				individual->push_back(static_cast<Tree::NodeP>(new Tree::Node(seq.at(r))));
			}
			// now, copy the rest of the original elements
			for (uint r = 0; r < copy.size(); r++){
				individual->push_back(static_cast<Tree::NodeP>(new Tree::Node(copy.at(r))));
			}
			ECF_LOG(state, 5, "Transposed individual: \n" + pool[i]->toString());
			std::vector<int> x;
		}
	}
}

void AlgGEP::transposeDc(StateP state, const std::vector<IndividualP>& pool)
{
	// Do a test and check whether the Dc domain actually exists
	GEPChromosomeP test = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(0)->getGenotype());
	if (test->dcLength < 1){
		ECF_LOG(state, 5, "ERCs not used in the chromosome. Dc transposition aborted.");
		return; // Do nothing if ERCs are not used
	}
	for (uint i = 0; i < pool.size(); i++) {
		if (state_->getRandomizer()->getRandomDouble() <= transDcRate_) {
			pool.at(i)->fitness->setInvalid();
			ECF_LOG(state, 5, "Transposing the constant domain: \n" + pool.at(i)->toString());
			// first, choose a random gene in the individual 
			GEPChromosomeP individual = boost::static_pointer_cast<GEP::GEPChromosome> (pool.at(i)->getGenotype());
			uint invGene = state_->getRandomizer()->getRandomInteger(individual->genes);
			uint geneOffset = invGene * (individual->geneLength);
			uint dcStart = geneOffset + individual->headLength + individual->tailLength;
			// next, choose two random points in the gene which delimit the copied sequence
			uint bitSeq = state_->getRandomizer()->getRandomInteger(individual->dcLength);
			uint bitSeqSecond, tmp;
			do
			{
				bitSeqSecond = state_->getRandomizer()->getRandomInteger(individual->dcLength);
			} while (bitSeq == bitSeqSecond);

			if (bitSeq > bitSeqSecond)
			{
				tmp = bitSeq;
				bitSeq = bitSeqSecond;
				bitSeqSecond = tmp;
			}

			// shorten the sequence to the specified maximum length
			if (bitSeqSecond - bitSeq >= transMaxLength_){
				bitSeqSecond = bitSeq + (uint)transMaxLength_ - 1;
			}

			// next, choose the random point in the ERC area in which the sequence will be inserted 
			uint bitPos = state_->getRandomizer()->getRandomInteger(individual->dcLength);
			ECF_LOG(state, 5, "Transposing the sequence in gene [" + uint2str(invGene) + "] from point (" + uint2str(bitSeq) + ") to point (" + uint2str(bitSeqSecond) + ") into position (" + uint2str(bitPos) + ")");

			// make a copy of the entire Dc tail
			std::vector<Tree::NodeP> seq;
			for (uint j = dcStart; j < (dcStart + individual->dcLength); j++){
				seq.push_back(static_cast<Tree::NodeP>(new Tree::Node(individual->at(j))));
			}
			// replace the original sequence first with the transposon
			uint seqIdx = bitSeq;
			uint r;
			for (r = (dcStart + bitPos); r <= (dcStart + bitPos + bitSeqSecond - bitSeq) && r < (dcStart + individual->dcLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			// now, copy the original elements from the transposition point until the end of the Dc tail
			seqIdx = bitPos;
			for (; r < (dcStart + individual->dcLength); r++){
				individual->at(r) = static_cast<Tree::NodeP>(new Tree::Node(seq.at(seqIdx++)));
			}
			ECF_LOG(state, 5, "Transposed individual: \n" + pool[i]->toString());
		}
	}
}