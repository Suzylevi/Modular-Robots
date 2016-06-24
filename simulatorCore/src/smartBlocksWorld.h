/*
 * smartBlocksWorld.h
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#ifndef SMARTBLOCKSWORLD_H_
#define SMARTBLOCKSWORLD_H_

#include "openglViewer.h"
#include "world.h"
#include "vector3D.h"
#include "smartBlocksBlock.h"
#include "smartBlocksCapabilities.h"
#include "scheduler.h"

namespace SmartBlocks {

static const Vector3D defaultBlockSize{25.0, 25.0, 11.0};

class SmartBlocksWorld : public BaseSimulator::World {
protected:
    presence *targetGrid;
    SmartBlocksCapabilities *capabilities;
    int tabStatsData[10];
    int nbreStats;
    static const int numPickingTextures = 4; /* The number of picking textures defined for
                                                this type of catom,
                                                used to deduce selected Block / face */
    ObjLoader::ObjLoader *objBlock,*objBlockForPicking,*objRepere;

    virtual ~SmartBlocksWorld();
public:
    SmartBlocksWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
                     int argc, char *argv[]);

    GLuint idTextureFloor,idTextureDigits;

    static void deleteWorld();
    static SmartBlocksWorld* getWorld() {
        assert(world != NULL);
        return((SmartBlocksWorld*)world);
    }

    void printInfo() {
        cout << "I'm a SmartBlocksWorld" << endl;
    }

    virtual void addBlock(int blockId,
                          SmartBlocksBlockCode *(*smartBlockCodeBuildingFunction)(SmartBlocksBlock*),
                          const Cell3DPosition &pos,const Color &col);

    // PTHY: TODO: May need a fix (2D->3D)
    inline presence *getTargetGridPtr(short *gs)
        { memcpy(gs,lattice->gridSize.pt,2*sizeof(short)); return targetGrid; };
    inline presence getTargetGrid(int ix,int iy)
        { return targetGrid[iy*lattice->gridSize[0]+ix]; };
    inline void setTargetGrid(presence value,int ix,int iy)
        { targetGrid[iy*lattice->gridSize[0]+ix]=value; };
    void initTargetGrid();

    inline void setCapabilities(SmartBlocksCapabilities *capa) { capabilities=capa; };
    void getPresenceMatrix0(const PointCel &pos,PresenceMatrix &pm);
    void getPresenceMatrix(const PointCel &pos,PresenceMatrix &pm);
    inline SmartBlocksCapabilities* getCapabilities() { return capabilities; };

    void linkBlock(const Cell3DPosition &pos);
    void loadTextures(const string &str);
    virtual void glDraw();
    virtual void glDrawId();

    virtual void disconnectBlock(SmartBlocksBlock *block);
    virtual void connectBlock(SmartBlocksBlock *block);

    virtual void setSelectedFace(int n);
    virtual void menuChoice(int);

    void deleteBlock(SmartBlocksBlock *bb);

    bool isBorder(int x,int y);
    bool isSingle(int x,int y);
    int nbreWellPlacedBlock();
    void createStats(int);
    void addStat(int n,int v);
    void printStats();
};

inline void deleteWorld() {
    SmartBlocksWorld::deleteWorld();
}

inline SmartBlocksWorld* getWorld() { return(SmartBlocksWorld::getWorld()); }

} // SmartBlocks namespace

#endif /* SMARTBLOCKSWORLD_H_ */
