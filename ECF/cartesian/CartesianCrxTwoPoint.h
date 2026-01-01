#ifndef CartesianCrsTwoPoint_h
#define CartesianCrsTwoPoint_h

#include "ecf/ECF_base.h"


namespace CGP {
/**
 * \brief Cartesian genotype: Two point crossover operator
 */
    class CartesianCrxTwoPoint: public CrossoverOp
    {
    public:
        /**
        Crossing points are determined randomly between all integer values in genotype.
        Left of the first point and right of the second are values from the first parent,
        everything inbetween is from the second parent.
        */
        bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child);
        bool initialize(StateP);
        void registerParameters(StateP);
    };
    typedef std::shared_ptr<CartesianCrxTwoPoint> CartesianCrxTwoPointP;

}

#endif /* CartesianCrsTwoPoint_h */
