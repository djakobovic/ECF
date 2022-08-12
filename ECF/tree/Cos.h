#ifndef Cos_h
#define Cos_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Cos function primitive (Tree genotype)
 */
class Cos : public Primitive
{
public:
	Cos(void);
	void execute(void* result, Tree& tree);
	~Cos(void);
};

}
}

#endif 