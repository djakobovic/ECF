#include <ECF/ECF.h>
#include "TSPEvalOp.h"


int main(int argc, char **argv)
{
	StateP state (new State);

	TSPEvalOp* tsp = new TSPEvalOp();
	state->setEvalOp(tsp);

	//state->addAlgorithm((AlgorithmP) new Alg);

	state->initialize(argc, argv);
	state->run();

	return 0;
}



//
// this main() function iterates over multiple TSP instances and optimizes each one in turn
// TSP instance filenames are read from 'instances.txt'
//
/*
int main(int argc, char **argv)
{

	// open file with TSP instance names
	std::ifstream inFile("instances.txt");
	if(!inFile) {
		std::cerr << "Error opening file instances.txt!\n";
		return 1;
	}

	// read instances from file
	std::vector< std::string > instances;
	std::string fileName;
	while(getline(inFile, fileName))
		instances.push_back(fileName);


	// run for selected TSP instances
	for(uint instance = 1; instance <= instances.size(); instance++) {

		// read XML config
		std::ifstream fin(argv[1]);
		if (!fin) {
			std::cerr << "Error opening config file! ";
			return 1;
		}

		std::string xmlFile, temp;
		while (!fin.eof()) {
			getline(fin, temp);
			xmlFile += "\n" + temp;
		}
		fin.close();

		// set log and stats parameters
		std::string instanceName = instances[instance - 1];
		std::string logName = "log", statsName = "stats";
		if(instance < 10) {
			logName += "0";
			statsName += "0";
		}
		logName += uint2str(instance) + ".txt";
		statsName += uint2str(instance) + ".txt";

		// update in XML
		XMLResults results;
		XMLNode xConfig = XMLNode::parseString(xmlFile.c_str(), "ECF", &results);
		XMLNode registry = xConfig.getChildNode("Registry");

		XMLNode func = registry.getChildNodeWithAttribute("Entry", "key", "tsp.infile");
		func.updateText(instanceName.c_str());
		XMLNode log = registry.getChildNodeWithAttribute("Entry", "key", "log.filename");
		log.updateText(logName.c_str());
		XMLNode stats = registry.getChildNodeWithAttribute("Entry", "key", "batch.statsfile");
		stats.updateText(statsName.c_str());

		// write back
		std::ofstream fout(argv[1]);
		fout << xConfig.createXMLString(true);
		fout.close();


		// finally, run ECF on a single instance
		StateP state (new State);

		// set the evaluation operator
		state->setEvalOp(new TSPEvalOp);

		state->initialize(argc, argv);
		state->run();
	}

	return 0;
}
*/