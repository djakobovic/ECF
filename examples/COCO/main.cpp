#include <ECF/ECF.h>
#include "FunctionMinEvalOp.h"


//
// this main() function optimizes a single COCO function (Id set in config file)
// function Ids: noiseless 1-24, noisy 101-130
//

int main(int argc, char **argv)
{
	StateP state (new State);

	// set the evaluation operator
	state->setEvalOp(new FunctionMinEvalOp);

	state->initialize(argc, argv);
	state->run();

	return 0;
}



//
// this main() function iterates over multiple COCO functions and optimizes each one in turn
// function Ids: noiseless 1-24, noisy 101-130
//
/*
int main(int argc, char **argv)
{
	// run for selected COCO functions
	for(uint function = 1; function <= 24; function++) {

		// read XML config
		std::ifstream fin(argv[1]);
		if (!fin) {
			std::cerr << "Error opening config file! (" << argv[1] << ")\n";
			return 1;
		}

		std::string xmlFile, temp;
		while (!fin.eof()) {
			getline(fin, temp);
			xmlFile += "\n" + temp;
		}
		fin.close();

		// set log and stats parameters
		std::string funcName = uint2str(function);
		std::string logName = "log", statsName = "stats";
		if(function < 10) {
			logName += "0";
			statsName += "0";
		}
		logName += uint2str(function) + ".txt";
		statsName += uint2str(function) + ".txt";

		// update in XML
		XMLResults results;
		XMLNode xConfig = XMLNode::parseString(xmlFile.c_str(), "ECF", &results);
		XMLNode registry = xConfig.getChildNode("Registry");

		XMLNode func = registry.getChildNodeWithAttribute("Entry", "key", "coco.function");
		func.updateText(funcName.c_str());
		XMLNode log = registry.getChildNodeWithAttribute("Entry", "key", "log.filename");
		log.updateText(logName.c_str());
		XMLNode stats = registry.getChildNodeWithAttribute("Entry", "key", "batch.statsfile");
		stats.updateText(statsName.c_str());

		// write back
		std::ofstream fout(argv[1]);
		fout << xConfig.createXMLString(true);
		fout.close();


		// finally, run ECF on a single function
		StateP state (new State);

		// set the evaluation operator
		state->setEvalOp(new FunctionMinEvalOp);

		state->initialize(argc, argv);
		state->run();
	}

	return 0;
}
*/