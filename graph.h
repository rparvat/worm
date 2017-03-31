#pragma once

#include <vector>
#include <string>

using namespace std;

typedef pair<int, int> Point;

class Graph {
  public:
    float** halfProbs;
    int z, x_max, y_max;
    int x_min, y_min, desired_x_max, desired_y_max;

    float getEdgeWeight(Point point1, Point point2);
    vector<Point> getNeighbors(Point point);
    Graph(int z);
    ~Graph();
};

string getImageName(int z, int yblock, int xblock);
float** openImages(int z);
        

