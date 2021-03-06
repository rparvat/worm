#pragma once

#include <vector>
#include <string>
#include <map>

using namespace std;

extern int X_I_MAX, 
       Y_I_MAX, 
       X_MIN_DESIRED, 
       X_MAX_DESIRED, 
       Y_MIN_DESIRED,
       Y_MAX_DESIRED,
       BLOCK_SIZE,
       EM_BLOCK_SIZE,
       SEED_RADIUS;

extern bool SHOW_SEEDS, 
       USE_ALTERNATE;

extern float DEFAULT_PROBABILITY;
extern string OUTPUT_PATH, PROBS_PATH;

typedef pair<int, int> Point;

class Graph {
  public:
    float** halfProbs;
    int z, x_max, y_max;
    int x_min, y_min, desired_x_max, desired_y_max;

    virtual float getEdgeWeight(Point point1, Point point2);
    vector<Point> getNeighbors(Point point);
    Graph();
    Graph(int z, int edgePower, int blur);
    void zeroSeeds(map<int, vector<Point>> seeds, map<Point, int> radii);
    ~Graph();
    
    static Graph* getNewGraph(int z, int edgePower, int blur = 0);
};

class LogGraph: public Graph {
  public:
    float getEdgeWeight(Point point1, Point point2);
    LogGraph(int z, int blur);
};

vector<Point> condenseSeeds(map<int, vector<Point>>* seeds);
string getImageName(int z, int yblock, int xblock);
float** openImagesLog(int z, int blur, int edgePower = -1);
uint8_t** openEMImages(int z);

string getSiftImageName(int z, int y_i, int x_i, int delta);
void tileEMImages_SIFT(int z);
        

