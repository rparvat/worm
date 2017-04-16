#pragma once

#include <vector>
#include <string>

using namespace std;

extern int X_I_MAX, 
       Y_I_MAX, 
       X_MIN_DESIRED, 
       X_MAX_DESIRED, 
       Y_MIN_DESIRED,
       Y_MAX_DESIRED,
       BLOCK_SIZE;

extern float DEFAULT_PROBABILITY;

typedef pair<int, int> Point;

class Graph {
  public:
    float** halfProbs;
    int z, x_max, y_max;
    int x_min, y_min, desired_x_max, desired_y_max;

    virtual float getEdgeWeight(Point point1, Point point2);
    vector<Point> getNeighbors(Point point);
    Graph();
    Graph(int z, int edgePower);
    ~Graph();
    
    static Graph* getNewGraph(int z, int edgePower);
};

class LogGraph: public Graph {
  public:
    float getEdgeWeight(Point point1, Point point2);
    LogGraph(int z);
};

string getImageName(int z, int yblock, int xblock);
float** openImages(int z, int edgePower);
float** openImagesLog(int z);
uint8_t** openEMImages(int z);
        

