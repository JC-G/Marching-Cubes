#include "Octree.h"
#include "TestShape.h"

#include <iostream>
Timing Octree::chunkTimer;
Octree::~Octree()
{
    // std::cout << "Deleting Chunk:" << myDetailLevel << ":" << glm::to_string(myPosition) << std::endl;
    if (!isLeaf) {
        deleteChildren();
    } else {
        myChunk->tryDelete();
    }
}



Octree::Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G, Octree* parent, glm::uvec3 positionInParent)
:mySize(size),myPosition(position),myDetailLevel(detailLevel), myGenerator(G), myParent(parent), myPositionInParent(positionInParent)
{
    isLeaf = true;
    myChunk = nullptr;
    // std::cout << "Creating New Chunk:"<< myDetailLevel << ":" << glm::to_string(myPosition) << std::endl;
}

BrushBoundingBox Octree::getBoundingBox() {
    return BrushBoundingBox(myPosition,myPosition+mySize);
}

void Octree::split()
{
    if (flagged) {
        flagged = false;
    } else {
        if (hasChunk) {
            myChunk->tryDelete();
            hasChunk = false;
            myChunk = nullptr;
        }
        //split this octree into 8
        for(int i = 0; i <= 1; i++) {
            for(int j = 0; j <= 1; j++) {
                for(int k = 0; k <= 1; k++) {
                    Octree* thisChild = new Octree(0.5f * mySize,myPosition + mySize * 0.5f * glm::vec3(i,j,k), myDetailLevel + 1,myGenerator,this,glm::uvec3(i,j,k));
                    auto it = myBrushes.begin();
                    while (it != myBrushes.end()) {
                        if ((*it)->getBoundingBox().intersects(thisChild->getBoundingBox())) {
                            thisChild->insertBrush(*it);
                        }
                        it++;
                    }
                    myChildren[i][j][k] = thisChild;
                }
            }
        }
        isLeaf = false;
    }
}

void Octree::chop()
{
    isLeaf = true;
    deleteChildren();
}

void Octree::deleteChildren()
{
    for(int i = 0; i <= 1; i++) {
        for(int j = 0; j <= 1; j++) {
            for(int k = 0; k <= 1; k++) {
                //delete children
                delete myChildren[i][j][k];
            }
        }
    }
}



bool Octree::shouldSplit(glm::vec3 inPos)
{
    if (!isLeaf) {
        //do not split if we are already a branch
        return false;
    }
    if (myDetailLevel >= Config::get<int>("octree_max_depth")) {
        //never split beyond max depth
        return false;
    }
    if (glm::length(inPos - getCenter()) <= Config::get<float>("octree_lod_scale")*glm::pow(0.5,myDetailLevel)) {
        //split if the camera is close
        if (Config::get<bool>("camera_lod")) {
            return true;
        }
    }

    //physics objects
    //if there is a physics object intersecting our bounding box, split down to maximum LOD
    for (auto shape : TestShape::allShapes) {
        if (getBoundingBox().intersects(shape->getBoundingBox())) {
            return true;
        }
    }

    return false;
}

bool Octree::shouldChop(glm::vec3 inPos)
{
    if (Config::get<bool>("never_chop")) {
        return false;
    }
    if (isLeaf) {
        //do not chop if we are already a leaf
        return false;
    }    
    if (glm::length(inPos - getCenter()) <= Config::get<float>("octree_lod_scale") * glm::pow(0.5,myDetailLevel)) {
        //do not chop if the camera is close
        if (Config::get<bool>("camera_lod")) {
            return false;
        }
    }

    //physics objects
    for (auto shape : TestShape::allShapes) {
        if (getBoundingBox().intersects(shape->getBoundingBox())) {
            return false;
        }
    }

    // std::cout << "Chopping" <<std::endl;
    return true;
}


void Octree::generateMarchingChunk(int edgeIndex)
{
    float stride = Config::get<float>("chunk_size");
    if (myChunk) {
        myChunk->tryDelete();
    }
    std::vector<BrushParams> myParams;
    for (Brush* b : myBrushes) {
        myParams.push_back(b->getBrushParams());
    }
    myChunk = MarchingChunk::createMarchingChunk(myPosition,glm::vec3(stride),mySize/stride,myGenerator,edgeIndex, myDetailLevel,myParams);
    hasChunk = true;
    needsRegen = false;
}


void Octree::draw(GLuint VAO)
{
    if (isLeaf) {
        if (hasChunk) {
            myChunk->draw(VAO);
        }
    } else {
        for(int i = 0; i <= 1; i++) {
            for(int j = 0; j <= 1; j++) {
                for(int k = 0; k <= 1; k++) {
                    myChildren[i][j][k]->draw(VAO);
                }
            }
        }
    }
}

void Octree::drawBoundary(GLuint VAO)
{
    if (isLeaf) {
        if (hasChunk) {
            myChunk->drawBoundary(VAO);
        }
    } else {
        for(int i = 0; i <= 1; i++) {
            for(int j = 0; j <= 1; j++) {
                for(int k = 0; k <= 1; k++) {
                    myChildren[i][j][k]->drawBoundary(VAO);
                }
            }
        }
    }
}

glm::vec3 Octree::getCenter()
{
    return myPosition + mySize*0.5f;
}


unsigned int Octree::getEdgeIndex()
{
    unsigned int E = 0;
    Octree* neighbor;

    //Note the order is +-z, +-y, +-x - this is reversed to make use of bit shifting
    glm::ivec3 edgeNeighbors[6] = {
        glm::ivec3(0,0,1),
        glm::ivec3(0,0,-1),
        glm::ivec3(0,1,0),
        glm::ivec3(0,-1,0),
        glm::ivec3(1,0,0),
        glm::ivec3(-1,0,0)
    };
    for (int i = 0; i < 6; i++) {
        neighbor = getNeighbor(edgeNeighbors[i]);
        if (neighbor && !neighbor->isLeaf) {
            E++;
        }
        if (i != 5) E = E << 1;
    }
    return E;
}

//return the neighbor at the same LOD if one exists, or NULL if none exists
//whether this is a leaf or not determines the edge index
Octree* Octree::getNeighbor(glm::ivec3 relativePosition) {
    if (!myParent) {
        return NULL;
    }
    glm::ivec3 childPosition = relativePosition + glm::ivec3(myPositionInParent);
    if (glm::all(glm::greaterThanEqual(childPosition, glm::ivec3(0))) && 
        glm::all(glm::lessThanEqual(childPosition,glm::ivec3(1)))) {
        //return the parent child at this relative position
        return myParent->childFromVec3(childPosition);
    } else {
        //return the child of the parent neighbor
        Octree* neighbor = myParent->getNeighbor(relativePosition);
        if (!neighbor || neighbor->isLeaf) return NULL;
        //glm does not have integer mod, so we have to do this manually...
        glm::ivec3 cm2 = glm::abs(glm::ivec3(childPosition.x % 2, childPosition.y % 2, childPosition.z % 2));
        return neighbor->childFromVec3(cm2);
    }
}

void Octree::generateAllChunks(bool force) {
    //needed so we have the shape of the octree before generating chunks that rely on it
    unsigned int E = getEdgeIndex();

    if (isLeaf) {
        if (!hasChunk || E != edgeIndex || force || needsRegen) {
            edgeIndex = E;
            generateMarchingChunk(edgeIndex);
        }
    } else {
        for(int i = 0; i <= 1; i++) {
            for(int j = 0; j <= 1; j++) {
                for(int k = 0; k <= 1; k++) {
                    myChildren[i][j][k]->generateAllChunks();
                }
            }
        }
    }
}

float Octree::getIntersectionPoint(glm::vec3 origin, glm::vec3 direction) {
    //get minimum t-value such that the ray starting at origin, travelling in direction, intersects with a triangle in the octree
    
    float minT = std::numeric_limits<float>::max();
    if (!getBoundingBox().intersectsRay(origin,direction)) {
        return minT;
    }
    if (!isLeaf) {
        for(int i = 0; i <= 1; i++) {
            for(int j = 0; j <= 1; j++) {
                for(int k = 0; k <= 1; k++) {
                    minT = glm::min(minT,myChildren[i][j][k]->getIntersectionPoint(origin,direction));
                }
            }
        }
        return minT;
    }
    if (!myChunk) {
        return std::numeric_limits<float>::max();
    }
    if (!myChunk->hasGeometry()) {
        return std::numeric_limits<float>::max();
    }
    return myChunk->getIntersectionPoint(origin,direction);
    //chunk intersection point
}

void Octree::refresh(glm::vec3 inPos) {
    //Initially, pass through the entire octree and flag chunks that should be deleted, according to the chop condition
    //Or split, according to split condition
    //return true if something has split

    if (Config::get<bool>("log_chunk_time")) {
        chunkTimer.timeDiff();
    }

    bool needsRefinement = flagSplitPhase(inPos);

    //Refine, undoing flags rather than splitting
    //need many passes, because a refinement may cause inconsistencies elsewhere
    if (needsRefinement) {
        bool done = false;
        int steps = 0;
        while (!done) {
            steps++;
            done = refine();
        }
    }
    //Now delete flagged chunks. Since this reaches all leaves, also do editing regeneration here
    deleteRegenPhase();

    if (Config::get<bool>("log_chunk_time")) {
        long t = chunkTimer.timeDiff();
        std::cout << "Octree took: " << t;
    }

    generateAllChunks();
    
    Editing::newBrushes.clear();
    if (Config::get<bool>("log_chunk_time")) {
        long t = chunkTimer.timeDiff();
        std::cout << ", Chunk took: " << t << std::endl;
    }

}

Octree* Octree::childFromVec3(glm::ivec3 pos) {
    return myChildren[pos.x][pos.y][pos.z];
}

//refine the octree - if a neighbor directly adjacent, smaller by more than one exists, split this
//return true if no refinements were made
bool Octree::refine() {
    bool result = true;
    glm::ivec3 edgeNeighbors[6] = {
        glm::ivec3(0,0,1),
        glm::ivec3(0,0,-1),
        glm::ivec3(0,1,0),
        glm::ivec3(0,-1,0),
        glm::ivec3(1,0,0),
        glm::ivec3(-1,0,0)
    };
    for (int n = 0; n < 6; n++) {
        Octree* neighbor = getNeighbor(edgeNeighbors[n]);
        if (neighbor && !neighbor->isLeaf) {
            for (int i = 0; i <= 1; i++) {
                for (int j = 0; j <= 1; j++) {
                    for (int k = 0; k <= 1; k++) {
                        glm::ivec3 childPosition = glm::ivec3(
                            edgeNeighbors[n].x == 0 ? i : (1-edgeNeighbors[n].x)/2,
                            edgeNeighbors[n].y == 0 ? j : (1-edgeNeighbors[n].y)/2,
                            edgeNeighbors[n].z == 0 ? k : (1-edgeNeighbors[n].z)/2
                        );
                        Octree* child = neighbor->childFromVec3(childPosition);
                        //if child exists, and is not a leaf, then this chunk is inconsistent
                        //if it is a leaf, split it, otherwise just unflag it
                        if (child && !child->isLeaf) {
                            if (isLeaf && myDetailLevel < Config::get<int>("octree_max_depth")) {
                                result = false;
                                split();
                                //once we know we are splitting, dont bother checking the other options
                                //break 4 loops is easiest with a goto
                            } else {
                                flagged = false;
                            }
                            goto REFINE_CHILDREN;
                        }
                        if (edgeNeighbors[n].z != 0) break;
                    }
                    if (edgeNeighbors[n].y != 0) break;
                }
                if (edgeNeighbors[n].x != 0) break;
            }
        }
    }
    
    REFINE_CHILDREN:
    if (!isLeaf) {
        for (int i = 0; i <= 1; i++) {
            for (int j = 0; j <= 1; j++) {
                for (int k = 0; k <= 1; k++) {
                    result &= myChildren[i][j][k]->refine();
                }
            }
        }
    }
    return result;
}

//returns true if something has been split or flagged - so refinement is needed
bool Octree::flagSplitPhase(glm::vec3 inPos) {
    bool result = false;
    if (shouldChop(inPos)) {
        flagged = true;
        result = true;
    } 
    if (shouldSplit(inPos)) {
        split();
        result = true;
    }
    if (!isLeaf) {
        for (int i = 0; i <= 1; i++) {
            for (int j = 0; j <= 1; j++) {
                for (int k = 0; k <= 1; k++) {
                    result |= myChildren[i][j][k]->flagSplitPhase(inPos);
                }
            }
        }
    }
    return result;
}

void Octree::deleteRegenPhase() {
    //chop chunks that shouldnt be there
    if (flagged) {
        chop();
        flagged = false;
    }
    if (!isLeaf) {
        for (int i = 0; i <= 1; i++) {
            for (int j = 0; j <= 1; j++) {
                for (int k = 0; k <= 1; k++) {
                    myChildren[i][j][k]->deleteRegenPhase();
                }
            }
        }
    } 
}


Octree* Octree::child(int i, int j, int k) {
    return myChildren[i][j][k];
}

void Octree::insertBrush(Brush* b) {
    myBrushes.push_back(b);
    if (isLeaf) {
        needsRegen = true;
        return;
    }
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                Octree* thisChild = myChildren[i][j][k];
                if (b->getBoundingBox().intersects(thisChild->getBoundingBox())) {
                    thisChild->insertBrush(b);
                }
            }
        }
    }
}