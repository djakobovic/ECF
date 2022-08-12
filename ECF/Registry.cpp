#include "ECF_base.h"


/**
 * Registers a parameter with the Registry.
 *
 * Called by each class wanting to use parameters, before configuration file is read.
 * \param	key          parameter name
 * \param	x            parameter value
 * \param	T            parameter type (of enum type)
 * \param	description  parameter description (optional) 
 * \param	index        parameter index (for multiple genotype entities with the same parameter name)
 * \return	true if new parameter is registered, false otherwise
 */
bool Registry::registerEntry(std::string key, voidP x, enum ECF::type T, std::string description, uint index)
{
	ParamP a = (ParamP) new ECF::Param;
	a->value= x;
	a->T = T;
	a->description = description;
	a->bModified=false;

	if(index > 0) 
		key = uint2str(index) + key;

	map_iter it = parameters_.find(key);
	if(it != parameters_.end())
		return false;

	parameters_.insert (std::pair < std::string, ParamP >(key, a));
	return true;
}


/**
 * Check whether a parameter is registered.
 * \param	key     parameter name
 */
bool Registry::isRegistered(std::string key)
{
	return parameters_.find(key) != parameters_.end();
}


/**
 * Stores new value for the parameter.
 * \param	key     parameter name
 * \param	x       pointer to new parameter value
 */
bool Registry::modifyEntry(std::string key, voidP x, uint index)
{
	if(index > 0) 
		key = uint2str(index) + key;

	map_iter it = parameters_.find(key);
	if(it != parameters_.end()) {
		it->second->value=x;
		it->second->bModified=true;
		return true;
	}
	else
		return false;
}


/**
 * Returns pointer to parameter value.
 * \param	key     parameter name
 * \param	index   parameter index (for multiple entities with the same parameter name)
 */
voidP Registry::getEntry(std::string key, uint index)
{
	if(index > 0) 
		key = uint2str(index) + key;

	map_iter iter = parameters_.find(key);

	if (iter != parameters_.end())
		return iter->second->value;

	voidP p;
	return p;
}


/**
 * Returns parameter type.
 * \param	key     parameter name
 * \return	parameter type (of enum type)
 */
ECF::type Registry::getType(std::string key)
{
	map_iter iter = parameters_.find(key);
	if (iter != parameters_.end())
		return iter->second->T;

	return ECF::ERROR;
}


/**
 * Check whether parameter value has been modified after initial parameter registration.
 * \param	key     parameter name
 * \param	index   parameter index (for multiple entities with the same parameter name)
 */
bool Registry::isModified(std::string key, uint index)
{
	if(index > 0) 
		key = uint2str(index) + key;

	map_iter iter = parameters_.find(key);

	if (iter != parameters_.end())
		return iter->second->bModified;

	return false;
}


std::string q(const std::string& s) { return "\"" + s + "\""; }

void print_tree(const pt::ptree& pt, int level)
{
    const std::string sep(2 * level, ' ');
    BOOST_FOREACH(const pt::ptree::value_type &v, pt) {
        std::cout
            << sep
            << q(v.first) << " : " << q(v.second.data()) << "\n";
        print_tree(v.second, level + 1);
    }
}

void print_tree(const pt::ptree& pt) { print_tree(pt, 0); }


/**
 * Reads entries from an XML node.
 *
 * The context of different configuration nodes (Algorithm, Genotype, Registry) is set with the prefix parameter.
 * \param	node    XML node
 * \param	prefix  configuration node context: parameter name is prefix + key
 * \param	index   parameter index (for multiple entities with the same parameter name)
 */
bool Registry::readEntries(const XMLNode& node, std::string prefix, uint index)
{
/*
	uint num = node.count(NODE_ENTRY);

	//std::cout << node.get_child("Entry.<xmlattr>.key").data() << std::endl;

	//print_tree(node);
	//pt::ptree entry = node.get_child(NODE_ENTRY);
	//pt::ptree::data_type& data = entry.data();
	//std::cout << v.first << std::endl;
	//std::cout << v.second.data() << std::endl;

		//pt::ptree& attributes = node.get_child("<xmlattr>.key");
		//BOOST_FOREACH(pt::ptree::value_type &v, attributes){
		//	std::cout << "First: " << v.first.data() << " Second: " << v.second.data() << std::endl;
		//}

	// iterate all nodes, ignore ones not <Entry>
	BOOST_FOREACH(pt::ptree::value_type &v, node) {

		if(v.first == "<xmlcomment>")
			continue;

		if(v.first != NODE_ENTRY) {
			std::cout << "Warning: unknown entry! ('" << v.first << "')" << std::endl;
			continue;
		}

		pt::ptree entry = v.second;
		if(entry.count("<xmlattr>") == 0) {
			std::cout << "Error: no attribute 'key' in current entry! (node '" << prefix << "')" << std::endl;
			return false;
		}

		pt::ptree pkey = entry.get_child("<xmlattr>");
		std::string key = pkey.get("key", "");

		if(key == "") {
			std::cout << "Error: no attribute 'key' in current entry! (node '" << prefix << "')" << std::endl;
			return false;
		}

		key = (prefix.length() > 0 ? prefix + "." : "") + key;

		if (index > 0)
			key = uint2str(index) + key;

		if (!isRegistered(key)) {
			std::cerr << "Warning: key " << key << " not registered." << std::endl;
			continue;
		}

		ECF::type t = getType(key);
		voidP new_value;

		std::string value = entry.get_value<std::string>("");

		if(value == "") {
			std::cout << "Error: no text in Entry node! (key: " << key << ")" << std::endl;
			return false;
		}

		bool ok = true;
		switch (t) {
			case ECF::INT:
				{
					int *x = new int;
					*x = str2int(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::UINT:
				{
					uint *x = new uint;
					*x = str2uint(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::STRING:
				{
					std::string *s = new std::string(value);
					new_value = (voidP)s;
					break;
				}
			case ECF::FLOAT:
				{
					float *x = new float;
					*x = str2flt(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::DOUBLE:
				{

					double *x = new double;
					*x = str2dbl(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::CHAR:
				{
					char *x = new char;
					*x = value[0];
					new_value = (voidP)x;

					break;
				}
			default:
				ok = false;
				break;
		}

		if (ok)
			modifyEntry(key, new_value);
	}
*/


	int num = node.nChildNode(NODE_ENTRY), iter = 0;

	for (int i = 0; i < num; ++i) {
		XMLNode child = node.getChildNode(NODE_ENTRY, &iter);
		if(!child.getAttribute("key")) {
			std::cerr << "Error: no attribute 'key' in current entry! (node '" << node.getName() << "')" << std::endl;
			return false;
		}

		std::string  key   = (prefix.length() > 0 ? prefix + "." : "") + child.getAttribute("key");
		if (child.getText() == NULL) {
			std::cerr << "Error: no text in Entry node! (key: " << key << ")" << std::endl;
			return false;
		}
		std::string  value = child.getText();

		if (index > 0)
			key = uint2str(index) + key;

		if (!isRegistered(key)) {
			std::cerr << "Warning: key " << key << " not registered." << std::endl;
			continue;
		}

		ECF::type t = getType(key);
		voidP new_value;

		bool ok = true;
		switch (t) {
			case ECF::INT:
				{
					int *x = new int;
					*x = str2int(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::UINT:
				{
					uint *x = new uint;
					*x = str2uint(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::STRING:
				{
					std::string *s = new std::string(value);
					new_value = (voidP)s;
					break;
				}
			case ECF::FLOAT:
				{
					float *x = new float;
					*x = str2flt(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::DOUBLE:
				{

					double *x = new double;
					*x = str2dbl(value);
					new_value = (voidP)x;

					break;
				}
			case ECF::CHAR:
				{
					char *x = new char;
					*x = value[0];
					new_value = (voidP)x;

					break;
				}
			default:
				ok = false;
				break;
		}

		if (ok)
			modifyEntry(key, new_value);
	}
	
	return true;
}


/**
 * Writes registry values in given XML node.
 * \param	xRegistry   XMLNode to write values to
 */
void Registry::write(XMLNode& xRegistry)
{
	xRegistry = XMLNode::createXMLTopNode(NODE_REGISTRY);

	for (map_iter it = parameters_.begin(); it != parameters_.end(); ++it) {

		if(!it->second->bModified)
			continue;

		XMLNode xParam = XMLNode::createXMLTopNode(NODE_ENTRY);
		std::string key   = it->first;
		voidP       value = it->second->value;
		ECF::type   T     = it->second->T;
		std::stringstream ss;

		switch (T) {
			case ECF::INT:
				ss << *((int *)value.get());
				break;
			case ECF::UINT:
				ss << *((uint *)value.get());
				break;
			case ECF::STRING:
				ss << *((std::string *)value.get());
				break;
			case ECF::FLOAT:
				ss << *((float *)value.get());
				break;
			case ECF::DOUBLE:
				ss << *((double *)value.get());
				break;
			case ECF::CHAR:
				ss << *((char *)value.get());
				break;
			default:
				break;
		}
		xParam.addAttribute("key", key.c_str());
		xParam.addText(ss.str().c_str());
		xRegistry.addChild(xParam);
	}
}


/**
 * Dumps the entire registry in given XML node.
 * \param	xRegistry   XMLNode to write values to
 */
void Registry::dumpEntries(XMLNode& xRegistry) 
{
	xRegistry = XMLNode::createXMLTopNode(NODE_REGISTRY);

	for (map_iter it = parameters_.begin(); it != parameters_.end(); ++it) {

		XMLNode xParam = XMLNode::createXMLTopNode(NODE_ENTRY);
		std::string key   = it->first;
		voidP       value = it->second->value;
		ECF::type   T     = it->second->T;
		std::string desc  = it->second->description;

		std::stringstream ss;

		switch (T) {
			case ECF::INT:
				ss << *((int *)value.get());
				break;
			case ECF::UINT:
				ss << *((uint *)value.get());
				break;
			case ECF::STRING:
				ss << *((std::string *)value.get());
				break;
			case ECF::FLOAT:
				ss << *((float *)value.get());
				break;
			case ECF::DOUBLE:
				ss << *((double *)value.get());
				break;
			case ECF::CHAR:
				ss << *((char *)value.get());
				break;
			default:
				break;
		}
		xParam.addAttribute("key", key.c_str());
		xParam.addText(ss.str().c_str());
		xParam.addAttribute("desc", desc.c_str());
		xRegistry.addChild(xParam);

		//if(desc != "")
		//	xRegistry.addClear(desc.c_str(), "<!-- ", " -->");
	}	
}

