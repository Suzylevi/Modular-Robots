/*! @file target.cpp
 * @brief Defines a target configuration for reconfiguration algorithms,
 * several ways of defining the configuration are provided to the user.
 * @author Pierre Thalamy
 * @date 21/07/2016
 */

#include "target.h"
#include "utils.h"
#include "csgParser.h"
#include "csgUtils.h"
#include "csg.h"
#include "catoms3DWorld.h"

#include <algorithm>

namespace BaseSimulator {

using namespace BaseSimulator::utils;

TiXmlNode *Target::targetListNode = NULL;
TiXmlNode *Target::targetNode = NULL;

Target *Target::loadNextTarget() {
	if (Target::targetListNode) {
        // Move targetNode pointer to next target (or NULL if there is none)
        Target::targetNode = targetListNode->IterateChildren(targetNode);

        if (Target::targetNode) {
            TiXmlElement* element = Target::targetNode->ToElement();
            const char *attr = element->Attribute("format");
            if (attr) {
                string str(attr);
                if (str.compare("grid") == 0) {
                    return new TargetGrid(Target::targetNode);
                } else if (str.compare("csg") == 0) {
                    return new TargetCSG(Target::targetNode);
                } else if (str.compare("relativeGrid") == 0) {
                    return new RelativeTargetGrid(Target::targetNode);
                } else {
                    throw UnknownTargetFormatException();
                }
            }
        }
    }

    return NULL;
}


/************************************************************
 *                         Target
 ************************************************************/

ostream& operator<<(ostream& out,const Target *t) {
    t->print(out);
    return out;
}

/************************************************************
 *                      TargetGrid
 ************************************************************/

TargetGrid::TargetGrid(TiXmlNode *targetNode) : Target(targetNode) {
    TiXmlNode *cellNode = targetNode->FirstChild("cell");
    const char* attr;
    TiXmlElement *element;
    Cell3DPosition position;
    Color defaultColor = Color();
    Color color;

    // Parse individual cells
    while (cellNode) {
        element = cellNode->ToElement();
        color = defaultColor;

        attr = element->Attribute("position");
        if (attr) {
            string str(attr);
            int pos1 = str.find_first_of(','),
                pos2 = str.find_last_of(',');
            position.pt[0] = atoi(str.substr(0,pos1).c_str());
            position.pt[1] = atoi(str.substr(pos1+1,pos2-pos1-1).c_str());
            position.pt[2] = atoi(str.substr(pos2+1,str.length()-pos1-1).c_str());
        } else {
            cerr << "error: position attribute missing for target cell" << endl;
            throw TargetParsingException();
        }
        attr = element->Attribute("color");
        if (attr) {
            string str(attr);
            int pos1 = str.find_first_of(','),
                pos2 = str.find_last_of(',');
            color.set(atof(str.substr(0,pos1).c_str())/255.0,
                      atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0,
                      atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0);
        }
		OUTPUT << "add target " << position << "," << color << endl;
        addTargetCell(position, color);
        cellNode = cellNode->NextSibling("cell");
    } // end while (cellNode)

    // Parse lines of cells
    cellNode = targetNode->FirstChild("targetLine");
    while (cellNode) {
        int line = 0, plane = 0;
        element = cellNode->ToElement();
        color = defaultColor;
        attr = element->Attribute("color");
        if (attr) {
            string str(attr);
            int pos1 = str.find_first_of(','),
                pos2 = str.find_last_of(',');
            color.set(atof(str.substr(0,pos1).c_str())/255.0,
                      atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0,
                      atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0);
        }

        attr = element->Attribute("line");
        if (attr) {
            line = atoi(attr);
        }

        attr = element->Attribute("plane");
        if (attr) {
            plane = atoi(attr);
        }

        attr = element->Attribute("values");
        if (attr) {
            string str(attr);
            position.pt[0] = 0;
            position.pt[1] = line;
            position.pt[2] = plane;
            int n = str.length();
            for(int i=0; i<n; i++) {
                if  (str[i] == '1') {
                    position.pt[0] = i;
                    addTargetCell(position, color);
                }
            }
        }

        cellNode = cellNode->NextSibling("blocksLine");
    } // end while (cellNode)*/
}

bool TargetGrid::isInTarget(const Cell3DPosition &pos) const {
    return tCells.count(pos);
}

const Color TargetGrid::getTargetColor(const Cell3DPosition &pos) {
    if (!isInTarget(pos)) {
        cerr << "error: attempting to get color of undefined target cell" << endl;
        throw InvalidPositionException();
    }

    return tCells[pos];
}

void TargetGrid::addTargetCell(const Cell3DPosition &pos, const Color c) {
    tCells.insert(std::pair<const Cell3DPosition, const Color>(pos, c));
}

void TargetGrid::print(ostream& where) const {
    for(auto const& pair : tCells) {
        where << "<cell position=" << pair.first << " color=" << pair.second << " />" << endl;
    }
}

void TargetGrid::boundingBox(BoundingBox &bb) {
    throw BaseSimulator::utils::NotImplementedException();
}

void TargetGrid::highlight() {
    for (const auto& pair : tCells) {
        getWorld()->lattice->highlightCell(pair.first, pair.second);
    }
}

void TargetGrid::unhighlight() {
    for (const auto& pair : tCells) {
        getWorld()->lattice->unhighlightCell(pair.first);
    }
}


/************************************************************
 *                      RelativeTargetGrid
 ************************************************************/

bool RelativeTargetGrid::isInTarget(const Cell3DPosition &pos) const {
    if (!origin)
        throw MissingInitializationException();

    return TargetGrid::isInTarget(pos);
}

void RelativeTargetGrid::setOrigin(const Cell3DPosition &org) {
    assert(!tCells.empty());

    // relatifyAndPrint();
    origin = new Cell3DPosition(org);

    // Then update every relative position parsed from the configuration file to its absolute counterpart
    map<const Cell3DPosition, const Color> absMap;
    for (const auto& targetEntry : tCells) {
        absMap.insert(
            std::pair<const Cell3DPosition, const Color>(targetEntry.first + *origin,
                                                        targetEntry.second));
    }

    tCells = absMap;

    computeGeodesics(); // Will populate each cell's distance to the origin in hops
    
    if (!targetCellsInConstructionOrder) {
        targetCellsInConstructionOrder = new list<Cell3DPosition>();

        for (const auto &pair : tCells) {
            cout << pair.first << " -dist: " << geodesicToOrigin[pair.first] << endl;
            targetCellsInConstructionOrder->push_back(pair.first);            
        }

        targetCellsInConstructionOrder->
            sort([=](const Cell3DPosition& first, const Cell3DPosition& second){
                    // return geodesicToOrigin[first] < geodesicToOrigin[second];
                    // if (first.dist_euclid(*origin) < second.dist_euclid(*origin))
                    if (geodesicToOrigin[first] < geodesicToOrigin[second])
                        return true;
                    // else if (first.dist_euclid(*origin) > second.dist_euclid(*origin))
                    else if (geodesicToOrigin[first] > geodesicToOrigin[second])
                        return false;
                    else {
                        if (first[0] < second[0]) return true;
                        else if (first[0] > second[0]) return false;
                        else {
                            if (first[1] > second[1]) return true;
                            else if (first[1] < second[1]) return false;
                            else {
                                return first[2] < second[2];
                            }
                        }
                    }
                });
    }
}

void RelativeTargetGrid::computeGeodesics() {
    geodesicToOrigin[*origin] = 0;
        
    // BFS-parent of every connector
    std::map<Cell3DPosition, Cell3DPosition> parent;
    parent[*origin] = *origin;

    list<Cell3DPosition> queue;
    queue.push_back(*origin);

    list<Cell3DPosition>::iterator itCell;
    Lattice* lattice = static_cast<Lattice*>(getWorld()->lattice);
    
    while(!queue.empty()) {
        Cell3DPosition cell = queue.front();
        queue.pop_front();

        // Get all adjacent cells of dequeued cell.
        // If one of the adjacent cells has not been visited, mark
        //  it as visited and enqueue it
        for (const auto& nCell : lattice->getNeighborhood(cell))
        {
            if (isInTarget(nCell) && parent.find(nCell) == parent.end()) {                
                parent[nCell] = cell;
                geodesicToOrigin[nCell] = geodesicToOrigin[cell] + 1;
                
                queue.push_back(nCell);
            }
        }
    }
}

void RelativeTargetGrid::highlightByDistanceToRoot() const {
    if (!origin)
        throw MissingInitializationException();

    for (const auto& cell : *targetCellsInConstructionOrder) {
        short distColorIdx = geodesicToOrigin.find(cell)->second % NB_COLORS;
        getWorld()->lattice->highlightCell(cell, Colors[distColorIdx]);
    }
}

list<Cell3DPosition>* RelativeTargetGrid::getTargetCellsInConstructionOrder() {
    if (!origin)
        throw MissingInitializationException();

    return targetCellsInConstructionOrder;
}

void RelativeTargetGrid::removeTargetCell(const Cell3DPosition& tc) {
    tCells.erase(tc);
}

bool RelativeTargetGrid::reconfigurationIsComplete() const { return tCells.empty(); }

void RelativeTargetGrid::relatifyAndPrint() {
    cout << endl << "=== START RELATIFIED TARGET ===" << endl << endl;
    
    const auto& minPair =
        *std::min_element(tCells.begin(), tCells.end(),
                          [](const std::pair<Cell3DPosition, Color>& pair1,
                             const std::pair<Cell3DPosition, Color>& pair2) {
                              return Cell3DPosition::compare_ZYX(pair1.first, pair2.first);
                          });
    
    for (const auto &pair : tCells) {
        Cell3DPosition relCell = pair.first - minPair.first;
        cout << "<cell position=\"" << relCell.config_print() << "\" />" << endl;
    }

    cout << endl << "=== END RELATIFIED TARGET ===" << endl << endl;
}

/************************************************************
 *                      TargetCSG
 ************************************************************/

TargetCSG::TargetCSG(TiXmlNode *targetNode) : Target(targetNode) {
    TiXmlNode *cellNode = targetNode->FirstChild("csg");
    TiXmlElement *element = cellNode->ToElement();
    string str = element->Attribute("content");
    bool boundingBox=true;
    element->QueryBoolAttribute("boundingBox", &boundingBox);

    char* csgBin = CSGParser::parseCsg(str);
    CsgUtils csgUtils;
    csgRoot = csgUtils.readCSGBuffer(csgBin);
    csgRoot->toString();

    if (boundingBox) csgRoot->boundingBox(bb);
}

Vector3D TargetCSG::gridToWorldPosition(const Cell3DPosition &pos) const {
    Vector3D worldPosition;
    worldPosition.pt[3] = 1.0;
    worldPosition.pt[2] = M_SQRT2_2 * (pos[2] + 0.5);
    if (IS_EVEN(pos[2])) {
        worldPosition.pt[1] = (pos[1] + 0.5);
        worldPosition.pt[0] = (pos[0] + 0.5);
    } else {
        worldPosition.pt[1] = (pos[1] + 1.0);
        worldPosition.pt[0] = (pos[0] + 1.0);
    }
    worldPosition.pt[0] += bb.P0[0];
    worldPosition.pt[1] += bb.P0[1];
    worldPosition.pt[2] += bb.P0[2];
    return worldPosition;
}

bool TargetCSG::isInTarget(const Cell3DPosition &pos) const {
    Color color;
    return csgRoot->isInside(gridToWorldPosition(pos), color);
}

bool TargetCSG::isInTargetBorder(const Cell3DPosition &pos, double radius) const {
    Color color;

    return csgRoot->isInBorder(gridToWorldPosition(pos), color, radius);
}

void TargetCSG::boundingBox(BoundingBox &bb) {
    csgRoot->boundingBox(bb);
}

const Color TargetCSG::getTargetColor(const Cell3DPosition &pos) {
    Color color;
    if (!csgRoot->isInside(gridToWorldPosition(pos), color)) {
        cerr << "error: attempting to get color of undefined target cell" << endl;
        throw InvalidPositionException();
    }
    return color;
}

ostream& operator<<(ostream& f,const TargetGrid&tg) {
    for (const auto& pair : tg.tCells) {
        f << pair.first << " -- " << pair.second << endl;
    }
    
    return f;
}

} // namespace BaseSimulator
