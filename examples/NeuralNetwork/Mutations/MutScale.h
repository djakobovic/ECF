#ifndef MUTSCALE_H_
#define MUTSCALE_H_
#include <ecf/ECF.h>

/*namespace FloatingPoint
{*/

	class MutScale : public MutationOp
	{
	protected:
	public:
		bool mutate(GenotypeP gene);
		bool initialize(StateP);
		void registerParameters(StateP);
	};
	typedef std::shared_ptr<MutScale> MutGaussAllP;
//}
#endif /* MUTSCALE_H_ */