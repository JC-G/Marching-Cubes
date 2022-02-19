#include "Octree.h"
#include "TestShape.h"

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

BrushBoundingBox Octree::getBoundingBox() {
    return BrushBoundingBox(myPosition,myPosition+mySize);
}

void Octree::update(glm::vec3 inPos)
{
    if (shouldChop(inPos)) {
        chop(inPos);
    } else if (shouldSplit(inPos)) {
        split(inPos);
    }
    if (isLeaf) {
        //Check if this leaf node needs a new marching chunk because of editing
        if (Editing::newBrushes.size() > 0) {
            BrushBoundingBox myBB = getBoundingBox();
            for (Brush* b : Editing::newBrushes) {
                if (b->getBoundingBox().intersects(myBB)) {
                    needsRegen = true;
                }
            }
        }
    } else {
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
    if (hasChunk) {
        myChunk = nullptr;
        hasChunk = false;
    }
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
    if (myDetailLevel >= Config::get<int>("octree_max_depth")) {
        //never split beyond max depth
        return false;
    }
    if (glm::length(inPos - getCenter()) <= Config::get<float>("octree_lod_scale")*glm::pow(0.5,myDetailLevel)) {
        //split if the camera is close
        return true;
    }

    //physics objects
    //if there is a physics object inside, or our center is less than "physics_object_lod_scale" away, split
    for (auto pos : TestShape::getShapePositions()) {
        if (getBoundingBox().contains(pos) || 
            glm::length(pos-getCenter()) <= Config::get<float>("physics_object_lod_scale")) {
            return true;
        }
    }


    return false;
}

bool Octree::shouldChop(glm::vec3 inPos)
{
    if (isLeaf) {
        //do not chop if we are already a leaf
        return false;
    }

    
    if (glm::length(inPos - getCenter()) <= Config::get<float>("octree_lod_scale") * glm::pow(0.5,myDetailLevel)) {
        //do not chop if the camera is close
        return false;
    }

    //physics objects
    //if there is a physics object inside, or our center is less than "physics_object_lod_scale" away, do not chop
    for (auto pos : TestShape::getShapePositions()) {
        if (getBoundingBox().contains(pos) || 
            glm::length(pos-getCenter()) <= Config::get<float>("physics_object_lod_scale")) {
            return false;
        }
    }

    return true;
}


void Octree::generateMarchingChunk(int edgeIndex)
{
    float stride = Config::get<float>("chunk_size");
    myChunk = std::shared_ptr<MarchingChunk>(new MarchingChunk(myPosition,glm::vec3(stride),mySize/stride,myGenerator,edgeIndex, myDetailLevel));
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
//whether this is a leaf or not determines the edge code
Octree* Octree::getNeighbor(glm::ivec3 relativePosition)
{
    if (!myParent) {
        return NULL;
    }
    glm::ivec3 childPosition = relativePosition + glm::ivec3(myPositionInParent);
    if (glm::all(glm::greaterThanEqual(childPosition, glm::ivec3(0))) && 
        glm::all(glm::lessThanEqual(childPosition,glm::ivec3(1)))) {
        return myParent->childFromVec3(childPosition);
    } else {
        Octree* neighbor = myParent->getNeighbor(relativePosition);
        if (!neighbor || neighbor->isLeaf) return NULL;
        //glm does not have integer mod, so we have to do this manually...
        glm::ivec3 cm2 = glm::abs(glm::ivec3(childPosition.x % 2, childPosition.y % 2, childPosition.z % 2));
        return neighbor->childFromVec3(cm2);
    }

}

void Octree::generateAllChunks(bool force)
{
    //needed so we have the shape of the octree before generating chunks that rely on it
    unsigned int E = getEdgeIndex();

    if (isLeaf) {
        if (!hasChunk || E != edgeIndex || force || needsRegen) {
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
    //initially update
    update(inPos);
    //refine the octree - if a leaf neighbor of a particular octree cell is larger by more than one,
    //split the neighbor
    //ie: if parent.getNeighbor = NULL, split
    //if parent does not exist, we are in the root
    //
    
    // if (!myParent) {
    //     glm::ivec3 edgeNeighbors[6] = {
    //         glm::ivec3(0,0,1),
    //         glm::ivec3(0,0,-1),
    //         glm::ivec3(0,1,0),
    //         glm::ivec3(0,-1,0),
    //         glm::ivec3(1,0,0),
    //         glm::ivec3(-1,0,0)
    //     };
        
    //     for (int i = 0; i < 6; i++) {
    //         Octree* neighbor = myParent->getNeighbor(edgeNeighbors[i]);
    //         if (!neighbor) {
    //             //find the 
    //         }
    //     }
    // }


}

Octree* Octree::childFromVec3(glm::ivec3 pos) {
    return myChildren[pos.x][pos.y][pos.z];
}