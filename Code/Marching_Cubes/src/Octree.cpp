#include "Octree.h"


#include <iostream>
Octree::~Octree()
{
    if (!isLeaf) {
        deleteChildren();
    }
}


Octree::Octree(glm::vec3 size, glm::vec3 position, int detailLevel, GeometryGenerator* G)
:mySize(size),myPosition(position),myDetailLevel(detailLevel), myGenerator(G)
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
        updateChildren(inPos);
    } else if (!hasChunk) {
        generateMarchingChunk();
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
                myChildren[i][j][k] = new Octree(0.5f * mySize,myPosition + mySize * 0.5f * glm::vec3(i,j,k), myDetailLevel + 1,myGenerator);
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
    if (glm::length(getCenter() - inPos) >= Config::get<float>("octree_lod_scale") * glm::pow(0.5,myDetailLevel - 1)) {
        return true;
    }
    return false;
}


void Octree::generateMarchingChunk()
{
    float stride = Config::get<float>("chunk_size");
    myChunk = std::shared_ptr<MarchingChunk>(new MarchingChunk(myPosition,glm::vec3(stride),mySize/stride,myGenerator));
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

