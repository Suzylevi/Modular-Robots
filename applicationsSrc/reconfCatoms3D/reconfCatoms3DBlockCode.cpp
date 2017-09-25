#include <iostream>
#include "reconfCatoms3DBlockCode.h"
#include "catoms3DWorld.h"

#define CONSTRUCT_WAIT_TIME 50
#define SYNC_WAIT_TIME 100
#define SYNC_RESPONSE_TIME SYNC_WAIT_TIME
#define PLANE_WAIT_TIME 0
#define MSG_TIME 100

using namespace std;
using namespace Catoms3D;

ReconfCatoms3DBlockCode::ReconfCatoms3DBlockCode(Catoms3DBlock *host):Catoms3DBlockCode(host) {
    scheduler = getScheduler();
	catom = (Catoms3DBlock*)hostBlock;

    reconf = new Reconf(catom);
    syncNext = new SyncNext(catom, reconf);
    syncPrevious = new SyncPrevious(catom, reconf);
    syncPlane = new SyncPlane(catom, reconf);
    neighborhood = new Neighborhood(catom, reconf, syncNext, syncPrevious, buildNewBlockCode);
    neighborMessages = new NeighborMessages(catom, reconf, neighborhood, syncNext, syncPrevious, syncPlane);
    syncPlaneManager = new SyncPlaneManager(catom, reconf, syncPlane, neighborhood, neighborMessages);
}

ReconfCatoms3DBlockCode::~ReconfCatoms3DBlockCode() {
    delete reconf;
    delete neighborhood;
    delete syncNext;
    delete syncPrevious;
}

void ReconfCatoms3DBlockCode::startup() {
    if (!BlockCode::target->isInTarget(catom->position)) {
        catom->setColor(RED);
    }
    catom->setColor(LIGHTGREY);

    planningRun();
    //stochasticRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(CONSTRUCT_WAIT_TIME));
}

void ReconfCatoms3DBlockCode::planningRun() {
    if (neighborhood->isFirstCatomOfPlane()) {
        reconf->planeParent = true;

        if (catom->blockId == 1) {
            SyncPlane_node_manager::root->planeNumber = catom->position[2];
            reconf->syncPlaneNode = SyncPlane_node_manager::root;
            reconf->syncPlaneNodeParent = SyncPlane_node_manager::root;
        }
        else {
            ReconfCatoms3DBlockCode *neighborBlockCode = (ReconfCatoms3DBlockCode*)Catoms3DWorld::getWorld()->getBlockByPosition(catom->position.addZ(-1))->blockCode;
            reconf->syncPlaneNodeParent = neighborBlockCode->reconf->syncPlaneNode;
        }
        neighborMessages->init();
        //if (reconf->checkPlaneCompleted()) {
            //syncPlaneManager->planeFinished();
        //}
    }
    else if (neighborhood->isFirstCatomOfLine()) {
        neighborMessages->sendMessageToGetParentInfo();
    }
    else {
        neighborMessages->sendMessageToGetLineInfo();
    }
}
void ReconfCatoms3DBlockCode::stochasticRun() {
    //if (catom->blockId == 1)
        //srand(time(NULL));
    for (int i = 0; i < 100000; i++) {
        ReconfCatoms3DBlockCode *catom = (ReconfCatoms3DBlockCode*)Catoms3D::getWorld()->getBlockById(rand()%Catoms3D::getWorld()->getSize() + 1)->blockCode;
        if (catom->neighborhood->addFirstNeighbor())
            break;
    }
}
void ReconfCatoms3DBlockCode::processLocalEvent(EventPtr pev) {
	MessagePtr message;
	stringstream info;

	switch (pev->eventType) {
    case EVENT_NI_RECEIVE: {
      message = (std::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
        switch(message->id) {
            case NEW_CATOM_MSG_ID:
            {
                neighborMessages->handleNewCatomMsg(message);
                break;
            }
            case NEW_CATOM_PARENT_MSG_ID:
            {
                neighborMessages->handleNewCatomParentMsg(message);
                break;
            }
            case NEW_CATOM_RESPONSE_MSG_ID:
            {
                neighborMessages->handleNewCatomResponseMsg(message);
                neighborMessages->init();
                //neighborhood->canFill();
                break;
            }
            case NEW_CATOM_PARENT_RESPONSE_MSG_ID:
            {
                neighborMessages->handleNewCatomParentResponseMsg(message);
                neighborMessages->init();
                //neighborhood->canFill();
                break;
            }
            case LEFT_SIDE_COMPLETED_MSG_ID:
            {
                neighborMessages->handleLeftSideCompletedMsg(message);
                if (reconf->checkPlaneCompleted()) {
                   syncPlaneManager->planeFinished();
                }
                break;
            }
            case RIGHT_SIDE_COMPLETED_MSG_ID:
            {
                neighborMessages->handleRightSideCompletedMsg(message);
                if (reconf->checkPlaneCompleted()) {
                   syncPlaneManager->planeFinished();
                }
                break;
            }
            case SYNCNEXT_MESSAGE_ID:
            {
                shared_ptr<Sync_message> recv_message = static_pointer_cast<Sync_message>(message);
                syncNextMessage(recv_message);
                break;
            }
            case SYNCPREVIOUS_MESSAGE_ID:
            {
                shared_ptr<Sync_message> recv_message = static_pointer_cast<Sync_message>(message);
                syncPreviousMessage(recv_message);
                break;
            }
            case SYNCNEXT_RESPONSE_MESSAGE_ID:
            case SYNCPREVIOUS_RESPONSE_MESSAGE_ID:
            {
                shared_ptr<Sync_response_message> recv_message = static_pointer_cast<Sync_response_message>(message);
                syncResponse(recv_message);
                break;
            }
            case PLANE_FINISHED_MSG_ID:
            {
                if (!reconf->planeFinished) {
                    syncPlaneManager->planeFinished();
                    removeSeed();
                }
                break;
            }
            case PLANE_FINISHED_ACK_MSG_ID:
            {
                syncPlaneManager->planeFinishedAck();
                continueOtherSeeds();
                break;
            }
            case CANFILLLEFT_MESSAGE_ID:
            {
                if (BlockCode::target->isInTarget(catom->position.addX(-1)) &&
                        catom->getInterface(catom->position.addX(-1))->connectedInterface != NULL) {
                    CanFillLeftResponse_message *msg = new CanFillLeftResponse_message();
                    getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + MSG_TIME, msg, catom->getInterface(catom->position.addY(1))));
                }
            }
            case CANFILLLEFTRESPONSE_MESSAGE_ID:
            {
                neighborhood->addNeighborToLeft();
            }
            case CANFILLRIGHT_MESSAGE_ID:
            {
                if (BlockCode::target->isInTarget(catom->position.addX(1)) &&
                        catom->getInterface(catom->position.addX(1))->connectedInterface != NULL) {
                    CanFillRightResponse_message *msg = new CanFillRightResponse_message();
                    getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + MSG_TIME, msg, catom->getInterface(catom->position.addY(1))));
                }
            }
            case CANFILLRIGHTRESPONSE_MESSAGE_ID:
            {
                //neighborhood->addNeighborToRight();
            }
          }
      }
      break;
	}
}


void ReconfCatoms3DBlockCode::syncNextMessage(shared_ptr<Sync_message> recv_message)
{
    if (catom->position[0] == recv_message->goal[0] && catom->position[1] == recv_message->goal[1]) {
        syncNext->response(recv_message->origin);
    }
    else if (!reconf->isLineCompleted() && (reconf->isSeedNext() || reconf->isSeedPrevious()))
    {
        reconf->requestQueue.push(recv_message);
    }
    else if (reconf->needSyncToRightNext()) {
        reconf->setSeedNext();
        neighborhood->tryAddNeighbors();
        neighborhood->tryAddNextLineNeighbor();
        reconf->requestQueue.push(recv_message);
    }
    else if (reconf->needSyncToRightPrevious()) {
        syncNext->response(recv_message->origin);
    }
    else {
        syncNext->handleMessage(recv_message);
        std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_WAIT_TIME));
    }
}

void ReconfCatoms3DBlockCode::syncPreviousMessage(shared_ptr<Sync_message> recv_message)
{
    if (catom->position[0] == recv_message->goal[0] && catom->position[1] == recv_message->goal[1]) {
        syncPrevious->response(recv_message->origin);
    }
    else if (!reconf->isLineCompleted() && (reconf->isSeedNext() || reconf->isSeedPrevious()))
    {
        reconf->requestQueue.push(recv_message);
    }
    else if (reconf->needSyncToRightNext()) {
        syncPrevious->response(recv_message->origin);
    }
    else if (reconf->needSyncToRightPrevious()) {
        reconf->setSeedPrevious();
        neighborhood->tryAddNeighbors();
        neighborhood->tryAddPreviousLineNeighbor();
        reconf->requestQueue.push(recv_message);
    }
    else {
        syncPrevious->handleMessage(recv_message);
        std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_WAIT_TIME));
    }
}

void ReconfCatoms3DBlockCode::syncResponse(shared_ptr<Sync_response_message> recv_message)
{
    if (recv_message->origin == catom->position) {
        neighborhood->addNeighborsWithoutSync();
    }
    else {
        syncNext->handleMessageResponse(recv_message);
        std::this_thread::sleep_for(std::chrono::milliseconds(SYNC_RESPONSE_TIME));
    }
}

void ReconfCatoms3DBlockCode::continueOtherSeeds()
{
    int continueBlockId = SyncPlane_node_manager::root->canContinue(catom->position[2]);
    if (continueBlockId != 0) {
        ReconfCatoms3DBlockCode* otherCatom = (ReconfCatoms3DBlockCode*)Catoms3D::getWorld()->getBlockById(continueBlockId)->blockCode;
        otherCatom->neighborhood->addNeighborToNextPlane();
    }
    else {
        int nextId = SyncPlane_node_manager::root->isOk(catom->position[2]+1);
        if (nextId != 0) {
            ReconfCatoms3DBlockCode* otherCatom = (ReconfCatoms3DBlockCode*)Catoms3D::getWorld()->getBlockById(nextId)->blockCode;
            otherCatom->neighborhood->addNeighborToNextPlane();
        }
    }
}

void ReconfCatoms3DBlockCode::removeSeed()
{
    if (!reconf->planeParent) {
        if (catom->getInterface(catom->position.addZ(-1))->isConnected()) {
            ReconfCatoms3DBlockCode* otherCatom = (ReconfCatoms3DBlockCode*)Catoms3D::getWorld()->getBlockByPosition(catom->position.addZ(-1))->blockCode;
            SyncPlane_node_manager::root->remove(otherCatom->reconf->syncPlaneNode, otherCatom->reconf->syncPlaneNodeParent);
        }
    }
}

BlockCode* ReconfCatoms3DBlockCode::buildNewBlockCode(BuildingBlock *host) {
    return (new ReconfCatoms3DBlockCode((Catoms3DBlock*)host));
}
