#pragma once

#include "graph.h"
#include <map>
#include <mutex>


using namespace std;

extern float DEFAULT_DISTANCE;
extern int DEFAULT_SEED;
extern int UPDATES_PER_RECONCILE;

class DijkstraThread;

class Dijkstra {
    public:
        float** finalDists;
        int** assignments;
        Graph graph;
        
        Dijkstra(Graph inputGraph);
        void reconcile(DijkstraThread& thread);
    private:
        void initArrays();
        mutex mtx;
};



//cimg::CImg<unsigned char> openMembraneIndiv(int z, int y, int x); 
map<int, Point>* getSeeds(int z);
void reconstruct(int z);
