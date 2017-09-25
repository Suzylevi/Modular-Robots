/*
 *  neighborhood.h
 *
 *  Created on: 28 February 2017
 *  Author: Thadeu
 */

#ifndef NEIGHBOR_H_
#define NEIGHBOR_H_

#define CANFILLLEFT_MESSAGE_ID 12001
#define CANFILLLEFTRESPONSE_MESSAGE_ID 12002
#define CANFILLRIGHT_MESSAGE_ID 12003
#define CANFILLRIGHTRESPONSE_MESSAGE_ID 12004

#include "cell3DPosition.h"
#include "directions.h"
#include "../reconf.h"
#include "../sync/syncNext.h"
#include "../sync/syncPrevious.h"

class Neighborhood {
private:
    Catoms3D::Catoms3DBlock *catom;
    Reconf *reconf;
    SyncNext *syncNext;
    SyncPrevious *syncPrevious;
    BlockCodeBuilder blockCodeBuilder;

    bool addNeighbor(Cell3DPosition pos);

public:
    static int numberBlockedModules;
    Neighborhood(Catoms3D::Catoms3DBlock *catom, Reconf *reconf, SyncNext *sn, SyncPrevious *sp, BlockCodeBuilder bcb);

    void addNeighborsWithoutSync();
    void addAllNeighbors();
    bool addFirstNeighbor();
    void addNeighborToLeft();
    void addNeighborToRight();

    void tryAddNextLineNeighbor();
    void tryAddPreviousLineNeighbor();
    void tryAddNeighbors();

    bool isOnLeftBorder();
    bool isOnRightBorder();
    bool isFirstCatomOfLine();
    bool isFirstCatomOfPlane();

    void addNeighborToNextPlane();
    void addNeighborToPreviousPlane();

    void canFill();
    void sendMessageToAddLeft();
    void sendMessageToAddRight();
};

class CanFillLeft_message : public Message {
public:
    CanFillLeft_message() {
        this->id = CANFILLLEFT_MESSAGE_ID;
    }
};

class CanFillLeftResponse_message : public Message {
public:
    CanFillLeftResponse_message() {
        this->id = CANFILLLEFTRESPONSE_MESSAGE_ID;
    }
};

class CanFillRight_message : public Message {
public:
    CanFillRight_message() {
        this->id = CANFILLRIGHT_MESSAGE_ID;
    }
};

class CanFillRightResponse_message : public Message {
public:
    CanFillRightResponse_message() {
        this->id = CANFILLRIGHTRESPONSE_MESSAGE_ID;
    }
};

#endif /* NEIGHBORHOOD_H_ */
