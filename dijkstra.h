#pragma once

#include "graph.h"
#include <map>
#include <mutex>
#include <atomic>


using namespace std;

extern float DEFAULT_DISTANCE;
extern int DEFAULT_SEED;
extern int UPDATES_PER_RECONCILE;
extern int MAX_DISTANCE;
extern bool PARALLEL_WRITE;

class DijkstraThread;

class Dijkstra {
    public:
        float** finalDists;
        int** assignments;
        Graph& graph;
        uint64_t overwrites;
        uint64_t threadsDone;
        
        Dijkstra(Graph& inputGraph);
        void reconcile(DijkstraThread& thread);
        void saveDists();
        void saveSeeds();
        ~Dijkstra();
    private:
        void initArrays();
};



//cimg::CImg<unsigned char> openMembraneIndiv(int z, int y, int x); 
map<int, vector<Point>>* getSeeds(int z);
void reconstruct(int z, bool saveSeeds, bool saveDists, int edgePower, int blur = 0);
void saveProbs(int z, int blur);
void saveEM(int z);
