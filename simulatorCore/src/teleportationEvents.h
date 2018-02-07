/**
 * @file teleportationEvents.h
 * @brief This file implements the realization of a teleportation event for C3D modules.
 *   Its purpose is to serve as a placeholder motion to test algorithm while proper
 *  Catoms3D movement primitives are being developed.
 *
 *  Created on: 13/11/2017
 *      Author: Pierre Thalamy
 */

#ifndef TELEPORTATIONEVENTS_H_
#define TELEPORTATIONEVENTS_H_

#include "buildingBlock.h"
#include "events.h"

namespace BaseSimulator {

     /* enum MovementDirection { */
     /*      NORTH, // a move to the cell reachable by a rotation in the north direction (absolute orientation) */
     /*      SOUTH, */
     /*      EAST, */
     /*      WEST, */
     /*      NORTH_EAST, */
     /*      NORTH_WEST, */
     /*      SOUTH_EAST, */
     /*      SOUTH_WEST */
     /* }; */
     
//===========================================================================================================
//
//          TeleportationStartEvent  (class)
//
//===========================================================================================================

     class TeleportationStartEvent : public BlockEvent {
          /* MovementDirection movementDirection; */
          /* BuildingBlock *pivot; */
          Cell3DPosition finalPosition;
     public:
          /* TeleportationStartEvent(Time, BuildingBlock*, BuildingBlock*, MovementDirection); */
          TeleportationStartEvent(Time, BuildingBlock*, const Cell3DPosition &fpos);
          TeleportationStartEvent(TeleportationStartEvent *ev);
          ~TeleportationStartEvent();
          void consumeBlockEvent() {};
          void consume();
          const virtual string getEventName();
     };

//===========================================================================================================
//
//          TeleportationStopEvent  (class)
//
//===========================================================================================================

     class TeleportationStopEvent : public BlockEvent {
          Cell3DPosition finalPosition;
     public:
          TeleportationStopEvent(Time, BuildingBlock *block,const Cell3DPosition &fpos);
          ~TeleportationStopEvent();
          void consumeBlockEvent() {};
          void consume();
          const virtual string getEventName();
     };

//===========================================================================================================
//
//          TeleportationEndEvent  (class)
//
//===========================================================================================================

     class TeleportationEndEvent : public BlockEvent {
     public:
          TeleportationEndEvent(Time, BuildingBlock *block);
          TeleportationEndEvent(TeleportationEndEvent *ev);
          ~TeleportationEndEvent();
          void consumeBlockEvent() {};
          void consume();
          const virtual string getEventName();
     };

}
#endif /* TELEPORTATIONEVENTS_H_ */