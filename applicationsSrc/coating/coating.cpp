#include <iostream>

#include "catoms3DSimulator.h"
#include "catoms3DBlockCode.h"

#include "coatingBlockCode.hpp"

using namespace std;
using namespace Catoms3D;

int main(int argc, char **argv) {
    try
    {
        createSimulator(argc, argv, CoatingBlockCode::buildNewBlockCode);
        getSimulator()->printInfo();
        BaseSimulator::getWorld()->printInfo();
        deleteSimulator();
    }
    catch(std::exception const& e)
    {
        cerr << "Uncaught exception: " << e.what();
    }

    return 0;
}
