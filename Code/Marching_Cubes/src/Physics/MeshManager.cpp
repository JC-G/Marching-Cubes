#include "MeshManager.h"

MeshManager* MeshManager::manager;

MeshManager::MeshManager(){
    terminate = false;
    if (Config::get<bool>("physics_generation_thread")) {
        physicsThread = std::thread(&MeshManager::runThread,this);
        std::cout << "Running in MT mode" << std::endl;

    } else {
        std::cout << "Running in ST mode" << std::endl;
    }
}
MeshManager::~MeshManager() {
    terminate = true;
    physicsThread.join();
}

void MeshManager::runThread() {
    while (!terminate) {
        updateMeshes();
    }
}

void MeshManager::updateMeshes() {
    bool done = false;
    while (!done) {
        std::optional<ChunkMesh*> mesh = multiQueue.pop();
        if (mesh.has_value()) {
            if (mesh.value()) {
                //move from state initialized to generating
                int expected = CHUNKMESH_INITIALIZED;
                if (mesh.value()->state.compare_exchange_strong(expected, CHUNKMESH_GENERATING)) {
                    //std::cout << "Beginning to generate mesh" << std::endl;
                    //actually do the expensive bit
                    mesh.value()->generateMesh();
                    //std::cout << "Mesh Generation Complete" << std::endl;

                    //move from state generating to generated
                    //otherwise, if we are in future_delete, then set to removing 
                    int expected2 = CHUNKMESH_GENERATING;
                    if (mesh.value()->state.compare_exchange_strong(expected2, CHUNKMESH_GENERATED)) {
                        //std::cout << "Mesh Set to Generated, adding to main thread queue" << std::endl;
                        singleQueue.push(mesh.value());
                    } else {
                        //std::cout << "Mesh was in state " << expected2 << std::endl;
                        if (expected2 == CHUNKMESH_FUTURE_DELETE) {
                            // std::cout << "Mesh requires future delete" << std::endl;
                            int expected3 = CHUNKMESH_FUTURE_DELETE;
                            if (mesh.value()->state.compare_exchange_strong(expected3, CHUNKMESH_REMOVING)) {
                                //std::cout << "Set to delete" << std::endl;
                                singleQueue.push(mesh.value());
                            } else {
                                //std::cout << "Something went wrong - in state " << expected3 << std::endl;
                            }
                        } else {
                            //std::cout << "Something went wrong 2 - in state " << expected2 << std::endl;
                        }
                    }
                } else {
                    //std::cout << "Instead of init: " << expected << std::endl;
                    
                }
            } else {
                //std::cout << "Attempted to process a deleted mesh" << std::endl;
            }
        } else {
            done = true;
        }
    }
    //std::cout << "Done mesh update" << std::endl;
}

void MeshManager::updateMeshesSingleThreaded() {
    bool done = false;
    while (!done) {
        std::optional<ChunkMesh*> mesh = singleQueue.pop();
        if (mesh.has_value()) {
            if (mesh.value()) {
                //not deleted

                bool stateChanged = false;
                while (!stateChanged) {
                    int expected = -1; //invalid state
                    int currentState = mesh.value()->state.load();
                    if (currentState == CHUNKMESH_INITIALIZED) {
                        //chunk deleted before even generated...
                        expected = CHUNKMESH_INITIALIZED;
                        if (mesh.value()->state.compare_exchange_strong(expected,CHUNKMESH_REMOVED)) {
                            //std::cout << "Mesh deleted from INITIALIZED" << std::endl;
                            delete mesh.value();
                            stateChanged = true;
                        } else {
                            //std::cout << "Failed to delete from INITIALIZED" << std::endl;
                        }
                    } else if (currentState == CHUNKMESH_GENERATING) {
                        //cannot delete from here, so set to future_delete instead
                        expected = CHUNKMESH_GENERATING;
                        if (mesh.value()->state.compare_exchange_strong(expected, CHUNKMESH_FUTURE_DELETE)) {
                            //std::cout << "Chunk set to future delete" << std::endl;
                            stateChanged = true;
                        } else {
                            //std::cout << "Could not set to future delete" << std::endl;
                        }
                    } else if (currentState == CHUNKMESH_GENERATED) {
                        //state is "generated", so add to the physics world
                        expected = CHUNKMESH_GENERATED;
                        if (mesh.value()->state.compare_exchange_strong(expected,CHUNKMESH_INWORLD)) {
                            //std::cout << "Adding chunk to world" << std::endl;
                            mesh.value()->addToWorld();
                            stateChanged = true;
                        } else {
                            //std::cout << "Could not add to world" << std::endl;
                        }
                    } else if (currentState == CHUNKMESH_INWORLD) {
                        //remove mesh from world
                        expected = CHUNKMESH_INWORLD;
                        if (mesh.value()->state.compare_exchange_strong(expected,CHUNKMESH_REMOVED)) {
                            mesh.value()->removeFromWorld();
                            delete mesh.value();
                            stateChanged = true;
                        } else {
                            //std::cout << "Could not remove from world" << std::endl;
                        }
                    } else if (currentState == CHUNKMESH_REMOVING) {
                        //delete from generated, but not in world
                        expected = CHUNKMESH_REMOVING;
                        if (mesh.value()->state.compare_exchange_strong(expected, CHUNKMESH_REMOVED)) {
                            //std::cout << "Cleaning up" << std::endl;
                            mesh.value()->cleanUp();
                            delete mesh.value();
                            stateChanged = true;
                        } else {
                            //std::cout << "could not clean up" <<std::endl;
                        }

                    }
                    
                    else {
                        //std::cout << "State was actually - " << currentState << std::endl;
                        
                    }
                }

            } else {
                //std::cout << "Physics Error - adding deleted body to world" << std::endl;
            }
        } else {
            done = true;
        }
    }
}

void MeshManager::addTask(ChunkMesh* item) {
    multiQueue.push(item);
}

void MeshManager::addTaskSingleThreaded(ChunkMesh* item) {
    //std::cout << "Adding task for " << item << std::endl;
    singleQueue.push(item);
}