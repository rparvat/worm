#include "graph.h"

#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

static string worm_path = "/home/heather/worm/";
static int X_MAX = 38912;
static int Y_MAX = 50176;

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

float Graph::getEdgeWeight(Point point1, Point point2)
{
    float probSum = halfProbs[point1.first][point1.second] 
        + halfProbs[point2.first][point2.second];
    return probSum;
}

vector<Point> Graph::getNeighbors(Point point)
{
    int x = point.first;
    int y = point.second;

    vector<Point> neighbors;
    neighbors.reserve(8);
    bool xTooLow, xTooHigh, yTooLow, yTooHigh;
    xTooLow = (x == 0);
    xTooHigh = (x == x_max - 1);
    yTooLow = (y == 0);
    yTooHigh = (y == y_max - 1);
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


