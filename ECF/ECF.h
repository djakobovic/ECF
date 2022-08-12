/*! \mainpage ECF - Evolutionary Computation Framework
 *
 * The information about ECF installation and usage can be found in help/install.html and help/tutorial.html
 *
 * See the list of examples (in modules) or class hierarchy for more information on ECF components.
 *
*/

/** \defgroup algorithms Algorithms
*/

/** \defgroup serial Sequential algorithms
	\ingroup algorithms
*/

/** \defgroup paralg Parallel algorithms
	\ingroup algorithms
*/

/** \defgroup genotypes Genotypes
*/

/** \defgroup evol Evolutionary Framework
*/

/** \defgroup main Main Classes
    \ingroup evol
*/

/** \defgroup evoop Evolutionary Operators
    \ingroup evol
*/

/** \defgroup term Termination Operators
    \ingroup evol
*/

/** \defgroup population Population
    \ingroup evol
*/

/** \defgroup examples Examples
*/



#ifndef ECF_h
#define ECF_h

#include <iostream>
#include <vector>
#include <cstdlib>
#include <boost/smart_ptr.hpp>
#include <boost/foreach.hpp>
#include "xml/xmlParser.h"

typedef boost::shared_ptr<void>  voidP;

const std::string ECF_VERSION = "1.5";

// base:
#include "ECF_base.h"

// derived:
#include "ECF_derived.h"

// genotypes:
#include "bitstring/BitString.h"
#include "binary/Binary.h"
#include "tree/Tree.h"
#include "permutation/Permutation.h"
#include "floatingpoint/FloatingPoint.h"
#include "apgenotype/APGenotype.h"
//#include "cartesian/Cartesian.h"

#endif // ECF_h

