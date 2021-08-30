#include "Octree.h"


#include <iostream>
Octree::~Octree()
{
    //dtor
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
                myChildren[i][j][k] = std::shared_ptr<Octree>(new Octree(0.5f * mySize,myPosition + mySize * 0.5f * glm::vec3(i,j,k), myDetailLevel + 1,myGenerator));
            }
        }
    }
    isLeaf = false;
}

void Octree::chop(glm::vec3 inPos)
{
    //do not delete children right now, just dont draw them
    isLeaf = true;
}

bool Octree::shouldSplit(glm::vec3 inPos)
{
    if (!isLeaf) {
        //do not split if we are already a branch
        return false;
    }
    //TODO - Split condition
    if ((myDetailLevel < 1) && glm::length(myPosition - inPos) <= 10) {
        return true;
    }
    return false;
}

bool Octree::shouldChop(glm::vec3 inPos)
{
    if (isLeaf) {
        //do not chop if we are already a leaf
        return false;
    }
    //TODO - Chop condition
    if (glm::length(myPosition - inPos) > 10) {
        return true;
    }
    return false;
}


void Octree::generateMarchingChunk()
{
    //TODO - decide stride
    myChunk = std::shared_ptr<MarchingChunk>(new MarchingChunk(myPosition,glm::vec3(32),mySize/32.0f,myGenerator));
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

