#include "XCSParams.h"

void XCSParams::registerParams(RegistryP registry) {
	
	//Default values taken from AlgXCS.cpp [1]

	registerParameter(registry, "beta", (voidP) new double(0.15), ECF::DOUBLE);
	registerParameter(registry, "alpha", (voidP) new double(0.1), ECF::DOUBLE);
	registerParameter(registry, "eps0", (voidP) new double(10), ECF::DOUBLE);
	registerParameter(registry, "accExp", (voidP) new double(5), ECF::DOUBLE);
	registerParameter(registry, "gama", (voidP) new double(0.71), ECF::DOUBLE);
	registerParameter(registry, "thresholdGA", (voidP) new uint(30), ECF::UINT);
	registerParameter(registry, "thresholdDel", (voidP) new double(20), ECF::DOUBLE);
	registerParameter(registry, "delta", (voidP) new double(0.1), ECF::DOUBLE);
	registerParameter(registry, "thresholdSub", (voidP) new double(20), ECF::DOUBLE);
	registerParameter(registry, "pDontCare", (voidP) new double(0.33), ECF::DOUBLE);
	registerParameter(registry, "pExplore", (voidP) new double(0.5), ECF::DOUBLE);
	registerParameter(registry, "mna", (voidP) new uint(1), ECF::UINT);
	registerParameter(registry, "maxPopSize", (voidP) new uint(1000), ECF::UINT);

	registerParameter(registry, "pCrossover", (voidP) new double(0.7), ECF::DOUBLE);
	registerParameter(registry, "pMutation", (voidP) new double(0.03), ECF::DOUBLE);
}

void XCSParams::readParams(RegistryP registry) {

	voidP vP = getParameterValue(registry, "mna");
	mna_ = *((uint*) vP.get());

	vP = getParameterValue(registry, "pExplore");
	p_explore_ = *((double*) vP.get());
	vP = getParameterValue(registry, "beta");
	beta_ = *((double*) vP.get());
	vP = getParameterValue(registry, "thresholdGA");
	thresholdGA_ = *((uint*) vP.get());
	vP = getParameterValue(registry, "gama");
	gama_ = *((double*) vP.get());

	vP = getParameterValue(registry, "eps0");
	eps0_ = *((double*) vP.get());
	vP = getParameterValue(registry, "alpha");
	alpha_ = *((double*) vP.get());
	vP = getParameterValue(registry, "accExp");
	accExp_ = *((double*) vP.get());
	vP = getParameterValue(registry, "pDontCare");
	pdc_ = *((double*) vP.get());

	vP = getParameterValue(registry, "thresholdDel");
	thresholdDel_ = *((double*) vP.get());
	vP = getParameterValue(registry, "delta");
	delta_ = *((double*) vP.get());
	vP = getParameterValue(registry, "thresholdSub");
	thresholdSub_ = *((double*) vP.get());
	vP = getParameterValue(registry, "maxPopSize");
	popSize_ = *((uint*) vP.get());
	vP = getParameterValue(registry, "pCrossover");
	pCrossover_ = *((double*) vP.get());
	vP = getParameterValue(registry, "pMutation");
	pMutation_ = *((double*) vP.get());
}

XMLNode XCSParams::getXMLNode() {

	XMLNode node = XMLNode::createXMLTopNode(name_.c_str(),0);
	XMLNode child = XMLNode::createXMLTopNode(NODE_ENTRY);
	
	std::stringstream ss;
	ss << alpha_; child.addAttribute("key", "alpha"); child.addText(ss.str().c_str()); 
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << beta_; child.addAttribute("key", "beta"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << gama_; child.addAttribute("key", "gama"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << delta_; child.addAttribute("key", "delta"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << p_explore_; child.addAttribute("key", "pExplore"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << thresholdGA_; child.addAttribute("key", "thresholdGA"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << thresholdDel_; child.addAttribute("key", "thresholdDel"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << thresholdSub_; child.addAttribute("key", "thresholdSub"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << eps0_; child.addAttribute("key", "eps0"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << accExp_; child.addAttribute("key", "accExp"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << pdc_; child.addAttribute("key", "pDontCare"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << popSize_; child.addAttribute("key", "maxPopSize"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << mna_; child.addAttribute("key", "mna"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	
	ss << pCrossover_; child.addAttribute("key", "pCrossover"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	ss << pMutation_; child.addAttribute("key", "pMutation"); child.addText(ss.str().c_str());
	node.addChild(child); ss.str(""); child = XMLNode::createXMLTopNode(NODE_ENTRY);
	
	return node;
}