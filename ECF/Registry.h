#ifndef Registry_h
#define Registry_h

#include <map>


#define NODE_ENTRY    "Entry"

namespace ECF
{

/**
 * \ingroup evol main
 * \brief Data types used for configuration file parameters.
 */
enum type
	{ INT, UINT, STRING, FLOAT, DOUBLE, CHAR, ERROR };


/**
 * \ingroup evol main
 * \brief ECF parameter structure, as stored in the Registry
 */
struct Param
{
	voidP value;
	enum ECF::type T;
	bool bModified;
	std::string description;
};

}
typedef std::shared_ptr<ECF::Param> ParamP;


/**
 * \ingroup evol main
 * \brief Repository for all the system parameters.
 *
 * The Registry stores all system parameters and parses sections of XML configuration file.
 */
class Registry 
{
private:
	std::map < std::string, ParamP > parameters_;
	typedef std::map < std::string, ParamP >::iterator map_iter;

public:
	bool registerEntry(std::string, voidP, enum ECF::type, std::string description = "", uint index = 0);
	bool isRegistered(std::string);
	bool modifyEntry(std::string, voidP, uint index = 0);
	voidP getEntry(std::string, uint index = 0);
	ECF::type getType(std::string);
	bool isModified(std::string, uint index = 0);
	bool readEntries(const XMLNode& node, std::string prefix = "", uint index = 0);
	void dumpEntries(XMLNode&);
	void write(XMLNode&);
};
typedef std::shared_ptr<Registry> RegistryP;

#endif 

