/*!
 * \file okteenSimulator.cpp
 * \brief okteen Simulator
 * \date 05/03/2015
 * \author Benoît Piranda
 */

#include <iostream>
#include "okteenSimulator.h"
#include <string.h>
#include "trace.h"
#include "utils.h"

using namespace std;
using namespace BaseSimulator::utils;

namespace Okteen {

void OkteenSimulator::help() {
	cerr << "VisibleSim:" << endl;
	cerr << "Okteen" << endl;
	exit(EXIT_SUCCESS);
}

OkteenSimulator::OkteenSimulator(int argc, char *argv[], BlockCodeBuilder bcb)
	: BaseSimulator::Simulator(argc, argv, bcb) {
	OUTPUT << "\033[1;34m" << "OkteenSimulator constructor" << "\033[0m" << endl;
}

OkteenSimulator::~OkteenSimulator() {
	OUTPUT << "\033[1;34m" << "OkteenSimulator destructor" << "\033[0m" <<endl;
}

void OkteenSimulator::createSimulator(int argc, char *argv[], BlockCodeBuilder bcb) {
	simulator =  new OkteenSimulator(argc, argv, bcb);
	simulator->parseConfiguration(argc, argv);
	simulator->startSimulation();
}

void OkteenSimulator::loadWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
				      int argc, char *argv[]) {
    world = new OkteenWorld(gridSize, gridScale, argc, argv);

	if (GlutContext::GUIisEnabled)
		world->loadTextures("../../simulatorCore/resources/textures/latticeTextures");

    World::setWorld(world);
}

void OkteenSimulator::loadBlock(TiXmlElement *blockElt, bID blockId, BlockCodeBuilder bcb,
								  const Cell3DPosition &pos, const Color &color, bool master) {

	// Any additional configuration file parsing exclusive to this type of block should be performed
	//  here, using the blockElt TiXmlElement.

	// set the orientation
	int orientation = 0;
	const char *attr = blockElt->Attribute("orientation");
	if (attr) {
		orientation = atoi(attr);
		OUTPUT << "orientation : " << orientation << endl;
	}

	// Finally, add block to the world
	((OkteenWorld*)world)->addBlock(blockId, bcb, pos, color, orientation, master);
}

} // Okteen namespace