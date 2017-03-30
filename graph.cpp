#include "graph.h"
#include "CImg.h"
#include "png.h"

#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

static string worm_path = "/home/heather/worm/";

static int X_I_MAX = 101631;
static int Y_I_MAX = 45567;

static int BLOCK_SIZE = 1024;
static int X_BLOCK_MAX = X_I_MAX / BLOCK_SIZE + 1;
static int Y_BLOCK_MAX = Y_I_MAX / BLOCK_SIZE + 1;

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
    /*
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
            array[x][y] = float(pixel) / 256.0 / 2.0;
        }
    }
    this->x_max = X_MAX;
    this->y_max = Y_MAX;
    */
    this->x_max = X_I_MAX;
    this->y_max = Y_I_MAX;
    this->halfProbs = openImages(z);
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

string my_itoa(int i, int d)
{
    string s = to_string(i);
    d -= s.size();
    for (int j = 0; j < d; j++)
    {
        s = "0" + s;
    }
    return s;
}

string getImageName(int z, int yblock, int xblock)
{
    string path = worm_path + "probs/";
    string dirName = "Block_" + my_itoa(z/100, 6)
        + "_" + my_itoa(yblock, 6)
        + "_" + my_itoa(xblock, 6)
        + "/";
    path = path + dirName + "forward/";
    string filename = my_itoa(z % 100, 4)
        + "-xnn-out-ch-01.png";
    return path + filename;
}

float** openImages(int z)
{
    float** array = new float*[X_I_MAX];
    for (int x = 0; x < X_I_MAX; x++)
    {
        array[x] = new float[Y_I_MAX];
        for (int y = 0; y < Y_I_MAX; y++)
        {
            array[x][y] = 1.0;
        }
    }

    for (int xblock = 1; xblock < X_BLOCK_MAX; xblock++)
    {
        for (int yblock = 1; yblock < Y_BLOCK_MAX; yblock++)
        {
            string filePath = getImageName(z, yblock, xblock);
            ifstream f(filePath.c_str());
            if (!f.good()) continue;

            cimg_library::CImg<short> image(filePath.c_str());
            for (int xind = 0; xind < BLOCK_SIZE; xind++)
            {
                int x_i = (xblock - 1) * BLOCK_SIZE + xind;
                for (int yind = 0; yind < BLOCK_SIZE; yind++)
                {
                    int y_i = (yblock - 1) * BLOCK_SIZE + yind;
                    if (image(xind, yind) > 255)
                    {
                        cout << image(xind, yind) << "\n";
                    }
                    array[x_i][y_i] = float(image(xind, yind)) / 256.0 / 2.0;
                }
            }
        }
    }
    return array;
}


