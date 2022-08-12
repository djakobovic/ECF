#ifndef BitString_h
#define BitString_h

#include "../Genotype.h"
#include "BitStringCrsOnePoint.h"
#include "BitStringMutSimple.h"
#include "BitStringMutMix.h"
#include "BitStringCrsUniform.h"
#include<sstream>

namespace BitString
{

/**
 * \defgroup bitstring BitString
 * \ingroup genotypes
 */

/**
 * \ingroup bitstring genotypes
 * \brief BitString class - implements genotype as a series of bits
 */
class BitString : public Genotype
{
protected:
	uint nBits_;

public:
	std::vector<bool> bits;

	BitString(uint nBits = 0)
	{	name_ = "BitString";
		nBits_ = nBits;
	}

	void registerParameters(StateP state)
	{
		if(nBits_ == 0)
			nBits_ = 10;
		registerParameter(state, "size", (voidP) new uint(nBits_), ECF::UINT,
			"number of bits (mandatory)");
	}

	bool initialize(StateP state)
	{
		if(!isParameterDefined(state, "size")) {
			ECF_LOG_ERROR(state, "Error: BitString genotype size not defined!");
			throw("");
		}

		voidP sizep = getParameterValue(state, "size");
		nBits_ = *((uint*) sizep.get());

		if(nBits_ < 1) {
			ECF_LOG_ERROR(state, "Error: invalid size for BitString genotype!");
			throw("");
		}

		bits.resize(nBits_);
		for(uint i = 0; i < bits.size(); i++){
			bits[i] = (state->getRandomizer()->getRandomInteger(2) % 2) ? true:false;
		}
		return true;
	}

	BitString* copy()
	{
		BitString *newObject = new BitString(*this);
		return newObject;
	}

	std::vector<CrossoverOpP> getCrossoverOp()
	{
		std::vector<CrossoverOpP> crx;
		crx.push_back(static_cast<CrossoverOpP> (new BitStringCrsOnePoint));
		crx.push_back(static_cast<CrossoverOpP> (new BitStringCrsUniform));
		return crx;
	}

	std::vector<MutationOpP> getMutationOp()
	{
		std::vector<MutationOpP> mut;
		mut.push_back(static_cast<MutationOpP> (new BitStringMutSimple));
		mut.push_back(static_cast<MutationOpP> (new BitStringMutMix));
		return mut;
	}

	void write(XMLNode &xBitString)
	{
		xBitString = XMLNode::createXMLTopNode("BitString");
		std::stringstream sValue;
		sValue << bits.size();
		xBitString.addAttribute("size", sValue.str().c_str());

		sValue.str("");
		for(uint iBit = 0; iBit < bits.size(); iBit++)
			sValue << ((bits[iBit] == true) ? '1' : '0');
		xBitString.addText(sValue.str().c_str());
	}

	void read(XMLNode &xBitString)
	{
		XMLCSTR xBits = xBitString.getText();
		for(uint iBit = 0; iBit < bits.size(); iBit++)
			bits[iBit] = (xBits[iBit] == '0') ? false : true;
	}
};
}

typedef boost::shared_ptr<BitString::BitString> BitStringP;

#endif // BitString_h

