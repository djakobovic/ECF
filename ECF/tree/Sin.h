#ifndef Sin_h
#define Sin_h
#include "Primitive.h"

namespace Tree {
namespace Primitives {

/**
 * \ingroup tree genotypes primitives
 * \brief Sin function primitive (Tree genotype)
 */
class Sin :	public Primitive
{
public:
	Sin(void);
	void execute(void* result, Tree& tree);
	~Sin(void);
};

}
}

#endif
