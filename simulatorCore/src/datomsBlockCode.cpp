/*!
 * \file datomsBlockCode.cpp
 * \brief deformable atoms BlockCode
 * \date 28/01/2018
 * \author Benoît Piranda
 */

#include <iostream>
#include "datomsBlockCode.h"
#include "network.h"
#include "trace.h"

using namespace std;

namespace Datoms {

DatomsBlockCode::DatomsBlockCode(DatomsBlock *host):BlockCode(host) {
	OUTPUT << "DatomsBlockCode constructor" << endl;
}

DatomsBlockCode::~DatomsBlockCode() {
	OUTPUT << "DatomsBlockCode destructor" << endl;
}

void DatomsBlockCode::addDebugAttributes(Scheduler *scheduler) {
 /*   DatomsBlock *catom = (DatomsBlock*)(hostBlock);
    scheduler->addKeyword(new KeywordT<short>("orientationCode",&catom->orientationCode,"orientation of the block"));*/
}

void DatomsBlockCode::processLocalEvent(EventPtr pev) {
    MessagePtr message;
    stringstream info;

cout << "event #" << pev->id << ":" << pev->eventType << endl;
    switch (pev->eventType) {
        case EVENT_NI_RECEIVE: {
            message = (std::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
    // search message id in eventFuncMap
            multimap<int,eventFunc>::iterator im = eventFuncMap.find(message->type);
            if (im!=eventFuncMap.end()) {
                P2PNetworkInterface *recv_interface = message->destinationInterface;
                (*im).second(this,message,recv_interface);
            } else {
                OUTPUT << "ERROR: message Id #"<< message->type << " unknown!" << endl;
            }
        } break;
        case EVENT_ADD_NEIGHBOR: {
            OUTPUT << "ADD_NEIGHBOR" << endl;
            //startup();
        } break;
        case EVENT_TAP: {
			int face = (std::static_pointer_cast<TapEvent>(pev))->tappedFace;
            onTap(face);
        } break;
        case EVENT_DEFORMATION_END: {
#ifdef verbose
			info.str("");
			info << "rec.: EVENT_MOTION_END";
			scheduler->trace(info.str(),hostBlock->blockId);
#endif
            DatomsBlock*c3d = (DatomsBlock*)hostBlock;
            c3d->setPositionAndOrientation(c3d->position,c3d->orientationCode);
            onMotionEnd();
        }  break;
    }
}


/*bool DatomsBlockCode::getAttribute(const string &att,ostringstream &sout) {

    DatomsBlock *catom = (DatomsBlock*)(hostBlock);

    if (att=="blockId") {
        sout << catom->blockId << endl;
        return false;
    } else if (att.substr(0,9)=="neighbor[") {
        int n = atoi(att.substr(9,2).c_str());
        if (n>=0 && n<12) {
            P2PNetworkInterface *p2p = catom->getInterface(n);
            if (p2p->connectedInterface!=NULL) {
                sout << "catom#" << p2p->connectedInterface->hostBlock->blockId << endl;
            } else {
                sout << "no catom linked."<< endl;
            }
        } else {
            sout << "Error: neighbor[n],  0<=n<12" << endl;
        }
        return true;
    }
    if (att=="gridpos") {
        sout << catom->position << endl;
        return false;
    }
    if (att=="worldpos") {
        sout << catom->getGlBlock()->position[0] << "," << catom->getGlBlock()->position[1] << "," << catom->getGlBlock()->position[2] << endl;
        return false;
    }
    if (att=="orientationcode") {
        sout << catom->orientationCode << endl;
        return false;
    }
    if (att=="matrix") {
        sout << catom->getGlBlock()->mat << endl;
        return false;
    }

    sout << "attribute "<< att << " unknown!" << endl;
    return false;
}
*/
}