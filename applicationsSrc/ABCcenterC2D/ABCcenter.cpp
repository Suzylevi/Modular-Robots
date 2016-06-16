#include <iostream>
#include "catoms2DSimulator.h"
#include "catoms2DBlockCode.h"
#include "ABCcenterCode.h"

using namespace std;
using namespace Catoms2D;

int main(int argc, char **argv) {
	createSimulator(argc, argv, ABCcenterCode::buildNewBlockCode);
	Catoms2DScheduler *scheduler = Catoms2D::getScheduler();

	getSimulator()->printInfo();
	scheduler->printInfo();
	BaseSimulator::getWorld()->printInfo();
	deleteSimulator();
	return(0);
}