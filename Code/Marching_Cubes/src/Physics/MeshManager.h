#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "LockQueue.h"
#include "ChunkMesh.h"

class MeshManager {
    public:
        MeshManager();
        ~MeshManager();
        std::thread physicsThread;
        void runThread();
        std::atomic_bool terminate;
        void addTask(ChunkMesh* mesh);

        void addTaskSingleThreaded(ChunkMesh* mesh);

        //called repeatedly by runThread if multithreading enabled,
        //otherwise, called every frame
        void updateMeshes();

        //ALWAYS called on the main thread
        void updateMeshesSingleThreaded();


        static MeshManager* manager;

    private:
        //Queue used exclusively for tasks performed on other thread(s)
        LockQueue<ChunkMesh*> multiQueue;
        //Queue used exclusively for tasks on the main thread
        LockQueue<ChunkMesh*> singleQueue;


};


#endif