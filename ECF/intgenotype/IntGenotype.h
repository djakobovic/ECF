#include "../Genotype.h"
#include <cmath>
#include <sstream>

#include "IntGenotypeCrxOp.h"
#include "IntGenotypeCrxTwoPoint.h"
#include "IntGenotypeCrxAverage.h"
#include "IntGenotypeMutOp.h"

namespace IntGenotype
{

	/**
	 * \defgroup intgenotype IntGenotype
	 * \ingroup genotypes
	 */

	 /**
	  * \ingroup intgenotype genotypes
	  * \brief IntGenotype class - implements genotype as a vector of int values
	  */
	class IntGenotype : public Genotype
	{
	public:
		int minValue_, maxValue_;
		uint nSize_;
		std::vector<int> intValues;

		IntGenotype()
		{
			name_ = "IntGenotype";
		}

		IntGenotype* copy()
		{
			IntGenotype *newObject = new IntGenotype(*this);
			return newObject;
		}

		/// return lower bound of the defined interval
		int getLBound()
		{
			return minValue_;
		}

		/// return upper bound of the defined interval
		int getUBound()
		{
			return maxValue_;
		}
		std::vector<CrossoverOpP> getCrossoverOp()
		{
			std::vector<CrossoverOpP> crx;
			crx.push_back(static_cast<CrossoverOpP> (new IntGenotypeCrxOp));
			crx.push_back(static_cast<CrossoverOpP> (new IntGenotypeCrxTwoPoint));
			crx.push_back(static_cast<CrossoverOpP> (new IntGenotypeCrxAverage));
			return crx;
		}

		std::vector<MutationOpP> getMutationOp()
		{
			std::vector<MutationOpP> mut;
			mut.push_back(static_cast<MutationOpP> (new IntGenotypeMutOp));
			return mut;
		}

		void registerParameters(StateP state);

		bool initialize(StateP state);

		void write(XMLNode &xIntGenotype);

		// mandatory if running parallel ECF or reading population from a milestone file
		void read(XMLNode& xIntGenotype);
	};
}

typedef boost::shared_ptr<IntGenotype::IntGenotype> IntGenotypeP;
