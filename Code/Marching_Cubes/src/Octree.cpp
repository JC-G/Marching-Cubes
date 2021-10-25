#include "Octree.h"


#include <iostream>
Octree::~Octree()
{
    if (!isLeaf) {
        deleteChildren();
    }
}


Octree::Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G, Octree* parent, glm::uvec3 positionInParent)
:mySize(size),myPosition(position),myDetailLevel(detailLevel), myGenerator(G), myParent(parent), myPositionInParent(positionInParent)
{
    isLeaf = true;
}

void Octree::update(glm::vec3 inPos)
{
    //std::cout << "inPos: " << glm::to_string(inPos) << std::endl;
    if (shouldChop(inPos)) {
        chop(inPos);
    } else if (shouldSplit(inPos)) {
        split(inPos);
    }
    if (!isLeaf) {
        //std::cout << "Updating Children" << std::endl;
        updateChildren(inPos);
    }
}

void Octree::updateChildren(glm::vec3 inPos)
{
    for(int i = 0; i <= 1; i++) {
        for(int j = 0; j <= 1; j++) {
            for(int k = 0; k <= 1; k++) {
                myChildren[i][j][k]->update(inPos);
            }
        }
    }
}


void Octree::split(glm::vec3 inPos)
{
    //split this octree into 8
    for(int i = 0; i <= 1; i++) {
        for(int j = 0; j <= 1; j++) {
            for(int k = 0; k <= 1; k++) {
                myChildren[i][j][k] = new Octree(0.5f * mySize,myPosition + mySize * 0.5f * glm::vec3(i,j,k), myDetailLevel + 1,myGenerator,this,glm::uvec3(i,j,k));
            }
        }
    }
    isLeaf = false;
}

void Octree::chop(glm::vec3 inPos)
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
    //TODO - Split condition
    if (myDetailLevel >= Config::get<int>("octree_max_depth")) {
        return false;
    }
    if (glm::length(inPos - getCenter()) <= Config::get<float>("octree_lod_scale")*glm::pow(0.5,myDetailLevel)) {
        return true;
    }
//    if ((myDetailLevel < 2) && glm::length(myPosition - inPos) <= 10) {
//        return true;
//    }
    return false;
}

bool Octree::shouldChop(glm::vec3 inPos)
{
    if (isLeaf) {
        //do not chop if we are already a leaf
        return false;
    }
    //TODO - Chop condition
    if (glm::length(inPos - getCenter()) >= Config::get<float>("octree_lod_scale") * glm::pow(0.5,myDetailLevel - 1)) {
        return true;
    }
    return false;
}


void Octree::generateMarchingChunk(int edgeIndex)
{
    float stride = Config::get<float>("chunk_size");
    myChunk = std::shared_ptr<MarchingChunk>(new MarchingChunk(myPosition,glm::vec3(stride),mySize/stride,myGenerator,edgeIndex));
    hasChunk = true;
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

glm::vec3 Octree::getCenter()
{
    return myPosition + mySize*0.5f;
}


unsigned int Octree::getEdgeIndex()
{
    unsigned int E = 0;
    Octree* neighbor;
    //+z
    neighbor = getNeighbor(glm::ivec3(0,0,1));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    E = E << 1;
    //-z
    neighbor = getNeighbor(glm::ivec3(0,0,-1));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    E = E << 1;
    //+y
    neighbor = getNeighbor(glm::ivec3(0,1,0));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    E = E << 1;
    //-y
    neighbor = getNeighbor(glm::ivec3(0,-1,0));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    E = E << 1;
    //+x
    neighbor = getNeighbor(glm::ivec3(1,0,0));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    E = E << 1;
    //-x
    neighbor = getNeighbor(glm::ivec3(-1,0,0));
    if (neighbor && !neighbor->isLeaf) {
        E++;
    }
    //E = E << 1;
    return E;
}

//return the neighbor at the same LOD if one exists, or NULL if none exists
//whether this is a leaf or not determines the edge code
Octree* Octree::getNeighbor(glm::ivec3 relativePosition)
{
    if (!myParent) {
        return NULL;
    }
    //-x
    if (relativePosition.x == -1) {
        if (myPositionInParent.x == 1) {
            //just return the octree node next to it
            return myParent->myChildren[0][myPositionInParent.y][myPositionInParent.z];
        } else {
            //return the "rightmost" node in the parents neighbor, or NULL if none at that LOD
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(-1,0,0));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[1][myPositionInParent.y][myPositionInParent.z];
        }
    }
    //+x
    if (relativePosition.x == 1) {
        if (myPositionInParent.x == 0) {
            //just return the octree node next to it
            return myParent->myChildren[1][myPositionInParent.y][myPositionInParent.z];
        } else {
            //return the "leftmost" node in the parents neighbor, or NULL if none at that LOD
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(1,0,0));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[0][myPositionInParent.y][myPositionInParent.z];
        }
    }
    //-y
    if (relativePosition.y == -1) {
        if (myPositionInParent.y == 1) {
            return myParent->myChildren[myPositionInParent.x][0][myPositionInParent.z];
        } else {
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(0,-1,0));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[myPositionInParent.x][1][myPositionInParent.z];
        }
    }
    //+y
    if (relativePosition.y == 1) {
        if (myPositionInParent.y == 0) {
            return myParent->myChildren[myPositionInParent.x][1][myPositionInParent.z];
        } else {
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(0,1,0));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[myPositionInParent.x][0][myPositionInParent.z];
        }
    }
    //-z
    if (relativePosition.z == -1) {
        if (myPositionInParent.z == 1) {
            return myParent->myChildren[myPositionInParent.x][myPositionInParent.y][0];
        } else {
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(0,0,-1));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[myPositionInParent.x][myPositionInParent.y][1];
        }
    }
    //+z
    if (relativePosition.z == 1) {
        if (myPositionInParent.z == 0) {
            return myParent->myChildren[myPositionInParent.x][myPositionInParent.y][1];
        } else {
            Octree* neighbor = myParent->getNeighbor(glm::ivec3(0,0,1));
            if (!neighbor || neighbor->isLeaf) return NULL;
            return neighbor->myChildren[myPositionInParent.x][myPositionInParent.y][0];
        }
    }
}

void Octree::generateAllChunks(bool force)
{
    //needed so we have the shape of the octree before generating chunks that rely on it
    unsigned int E = getEdgeIndex();

    if (isLeaf) {
        if (!hasChunk || E != edgeIndex || force) {
            edgeIndex = E;
            generateMarchingChunk(edgeIndex);
            // if (Config::get<bool>("debug") && edgeIndex != 0 && myChunk->hasGeometry()) {
            //     std::cout << "Chunk Status: " << myPosition.x << ", " << myPosition.y << ", " << myPosition.z << ": Edge Code: " << edgeIndex <<std::endl;
            // }
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


