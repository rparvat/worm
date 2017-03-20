#include "graph.h"

#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

string worm_path = "/home/heather/worm/";

int X_MAX = 37888;
int Y_MAX = 33792;

string zToString(int z) 
{
    int DEC_SIZE = 6;
    string s = to_string(z);
    for (auto i = 0; i < DEC_SIZE - s.length(); i++)
    {
        s = "0" + s;
    }
    return s;
}

Graph::Graph(int z)
{
    string path = worm_path + "probs_txt/";
    string filename = "full_z_" + zToString(z) + ".txt";
    ifstream in(filename, ios_base::in);
    float** array = new float*[X_MAX];
    for (int x = 0; x < X_MAX; x++)
    {
        array[x] = new float[Y_MAX];
        for (int y = 0; y < Y_MAX; y++)
        {
            int pixel;
            in >> pixel;
            array[x][y] = float(pixel) / 256.0;
        }
    }
    this->x_max = X_MAX;
    this->y_max = Y_MAX;
    this->halfProbs = array;
}

float Graph::getEdgeWeight(int x1, int y1, int x2, int y2)
{
    assert(((x1 == x2) && abs(y1 - y2) == 1) || ((y1 == y2) && abs(x1 - x2) == 1));
    float probSum = halfProbs[x1][y1] + halfProbs[x2][y2];
    return probSum;
}

vector<Point> Graph::getNeighbors(int x, int y)
{
    vector<Point> neighbors;
    bool xTooLow, xTooHigh, yTooLow, yTooHigh;
    xTooLow = x == 0;
    xTooHigh = x == x_max - 1;
    yTooLow = y == 0;
    yTooHigh = y == y_max - 1;
    if (!xTooLow)
    {
        if (!yTooLow) neighbors.push_back((Point(x - 1, y - 1)));
        neighbors.push_back(Point(x - 1, y));
        if (!yTooHigh) neighbors.push_back((Point(x - 1, y + 1)));
    }
    if (!yTooLow) neighbors.push_back((Point(x, y - 1)));
    if (!yTooHigh) neighbors.push_back((Point(x, y + 1)));
    if (!xTooHigh)
    {
        if (!yTooLow) neighbors.push_back((Point(x + 1, y - 1)));
        neighbors.push_back(Point(x + 1, y));
        if (!yTooHigh) neighbors.push_back((Point(x + 1, y + 1)));
    }
    return neighbors;
}


Graph::~Graph() 
{
    delete(halfProbs);
}


