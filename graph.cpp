#include "graph.h"
#include "CImg.h"
#include "png.h"
#include <opencv2/opencv.hpp>

#include <iostream>
#include <cassert>
#include <fstream>

using namespace std;

static string worm_path = "/home/heather/worm/";
static string alternate_probs_path = "/mnt/disk7/rajeev/xnn_output/";

// these are just the overall dimensions.
int X_I_MAX = 101631 / 2;
int Y_I_MAX = 45567;

// these give the area that we are interested in.
//int X_MIN_DESIRED = 18000; //X_I_MAX / 5 * 2;//0;
//int X_MAX_DESIRED = 24500; //X_I_MAX / 5 * 3;//X_I_MAX;
int X_MIN_DESIRED = 0;
int X_MAX_DESIRED = X_I_MAX;

//int Y_MIN_DESIRED = 1750; //Y_I_MAX / 12;//0;
//int Y_MAX_DESIRED = 7000; //Y_I_MAX / 12 * 2;//Y_I_MAX;
int Y_MIN_DESIRED = 0;
int Y_MAX_DESIRED = Y_I_MAX;

// these are helpers -- can ignore.
int BLOCK_SIZE = 1024;
int X_BLOCK_MAX = X_MAX_DESIRED / BLOCK_SIZE - 1;
int Y_BLOCK_MAX = Y_MAX_DESIRED / BLOCK_SIZE - 1;

int EM_BLOCK_SIZE = 512;

bool SHOW_SEEDS = true;
bool USE_ALTERNATE = false;

int SEED_RADIUS = 7;
float DEFAULT_PROBABILITY = 100000;

string OUTPUT_PATH = "/mnt/disk7/rajeev/algo_output/";
static string SIFT_PATH = "/mnt/disk7/rajeev/sift_tiles/";

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

vector<Point> condenseSeeds(map<int, vector<Point>>* seeds)
{
    vector<Point> allPoints;
    for (auto pair : *seeds)
    {
        for (Point point : pair.second)
        {
            allPoints.push_back(point);
        }
    }
    return allPoints;
}
    

// edgePower: -1 represents log edge weights
//            any positive integer represents k^th norm edge weights
//            all else: undefined
Graph* Graph::getNewGraph(int z, int edgePower, int blur)
{
    if (edgePower == -1) return new LogGraph(z, blur);
    if (edgePower >= 1) return new Graph(z, edgePower, blur);
    cout << "UNDEFINED EDGE POWER: " << edgePower  << "\n";
    return nullptr;
}

Graph::Graph(int zDesired, int edgePower, int blur)
{
    cout << "normal graph!!";
    this->z = zDesired;
    this->x_max = X_I_MAX;
    this->y_max = Y_I_MAX;

    this->x_min = X_MIN_DESIRED;
    this->y_min = Y_MIN_DESIRED;

    this->desired_x_max = X_MAX_DESIRED;
    this->desired_y_max = Y_MAX_DESIRED;
    this->halfProbs = openImages(z, edgePower, blur);
}

Graph::Graph() {};

float Graph::getEdgeWeight(Point point1, Point point2)
{
    return (halfProbs[point1.first][point1.second] + halfProbs[point2.first][point2.second])
        * sqrt(abs(point1.first - point2.first) + abs(point1.second - point2.second));
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

void Graph::zeroSeeds(map<int, vector<Point>> seeds, map<Point, int> radii)
{
    auto allPoints = condenseSeeds(&seeds);
    for (auto point : allPoints)
    {
        auto x = point.first;
        auto y = point.second;
        auto radius = radii[point];

        for (auto i = x - radius; i <= x + radius; i++)
        {
            for (auto j = y - radius; j <= y + radius; j++)
            {
                if (sqrt((i-x)*(i-x) + (j-y)*(j-y)) <= radius)
                {
                    halfProbs[i][j] = 0;
                }
            }
        }
    }
}


Graph::~Graph() 
{
    for (int i = 0; i < x_max; i++)
    {
        delete(halfProbs[i]);
    }
    delete(halfProbs);
}

LogGraph::LogGraph(int zDesired, int blur)
{
    cout << "log graph!! ";
    this->z = zDesired;
    this->x_max = X_I_MAX;
    this->y_max = Y_I_MAX;

    this->x_min = X_MIN_DESIRED;
    this->y_min = Y_MIN_DESIRED;

    this->desired_x_max = X_MAX_DESIRED;
    this->desired_y_max = Y_MAX_DESIRED;

    this->halfProbs = openImagesLog(z, blur);
}


float LogGraph::getEdgeWeight(Point point1, Point point2)
{
    return halfProbs[point2.first][point2.second];
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

string alternateGetImageName(int z, int yblock, int xblock)
{
    string path = alternate_probs_path + to_string(z) + "/";
    string filename = to_string(yblock) + "_" + to_string(xblock) + ".png";
    return path + filename;
}


float** openImages(int z, int edgePower, int blur)
{
    float** array = new float*[X_I_MAX];
    for (int x = 0; x < X_I_MAX; x++)
    {
        array[x] = new float[Y_I_MAX];
    }
    cilk_for (int x = 0; x < X_I_MAX; x++)
    {
        for (int y = 0; y < Y_I_MAX; y++)
        {
            array[x][y] = DEFAULT_PROBABILITY;
        }
    }

    auto minXBlock = X_MIN_DESIRED / BLOCK_SIZE;
    auto minYBlock = Y_MIN_DESIRED / BLOCK_SIZE;
    
    auto imageNameFunction = USE_ALTERNATE ? alternateGetImageName : getImageName;

    cilk_for (int xblock = minXBlock; xblock <= X_BLOCK_MAX; xblock++)
    {
        for (int yblock = minYBlock; yblock <= Y_BLOCK_MAX; yblock++)
        {
            string filePath = imageNameFunction(z, yblock, xblock);
            ifstream f(filePath.c_str());
            if (!f.good()) continue;

            cimg_library::CImg<short> image(filePath.c_str());
            if (blur) image.blur(float(blur), float(blur), float(0));
            for (int xind = 0; xind < BLOCK_SIZE; xind++)
            {
                int x_i = (xblock) * BLOCK_SIZE + xind;
                for (int yind = 0; yind < BLOCK_SIZE; yind++)
                {
                    int y_i = (yblock) * BLOCK_SIZE + yind;
                    array[x_i][y_i] = pow(
                            float(image(xind, yind) / 256.0),
                            edgePower);
                }
            }
        }
    }
    return array;
}

float** openImagesLog(int z, int blur)
{
    float** array = new float*[X_I_MAX];
    for (int x = 0; x < X_I_MAX; x++)
    {
        array[x] = new float[Y_I_MAX];
    }
    cilk_for (int x = 0; x < X_I_MAX; x++)
    {
        for (int y = 0; y < Y_I_MAX; y++)
        {
            array[x][y] = DEFAULT_PROBABILITY;
        }
    }

    auto minXBlock = X_MIN_DESIRED / BLOCK_SIZE;
    auto minYBlock = Y_MIN_DESIRED / BLOCK_SIZE;

    auto imageNameFunction = USE_ALTERNATE ? alternateGetImageName : getImageName;

    cilk_for (int xblock = minXBlock; xblock <= X_BLOCK_MAX; xblock++)
    {
        for (int yblock = minYBlock; yblock <= Y_BLOCK_MAX; yblock++)
        {
            string filePath = imageNameFunction(z, yblock, xblock);
            ifstream f(filePath.c_str());
            if (!f.good()) {
                //cout << "xblock " << xblock << " yblock " << yblock << " doesn't exist\n";
                continue;
            }

            cimg_library::CImg<short> image(filePath.c_str());
            if (blur) image.blur(float(blur), float(blur), float(0));
            for (int xind = 0; xind < BLOCK_SIZE; xind++)
            {
                int x_i = (xblock) * BLOCK_SIZE + xind;
                for (int yind = 0; yind < BLOCK_SIZE; yind++)
                {
                    int y_i = (yblock) * BLOCK_SIZE + yind;
                    array[x_i][y_i] = -float(
                            log(1.0 - image(xind, yind) / 256.0)
                            / log(2.0)
                            );
                }
            }
        }
    }
    return array;
}

string getEMImageName(int z, int yem, int xem)
{
    string path = "/mnt/disk10/heather/worm-project/data/";
    string dirName = to_string(z) + "/";
    path = path + dirName;
    string filename = to_string(yem) + "_" 
        + to_string(xem) + "_"
        + "1.jpg";
    return path + filename;
}

void tileEMImages_SIFT(int z)
{
    int x_block_min = X_MIN_DESIRED / EM_BLOCK_SIZE;
    int x_block_max = X_MAX_DESIRED / EM_BLOCK_SIZE;

    int y_block_min = Y_MIN_DESIRED / EM_BLOCK_SIZE;
    int y_block_max = Y_MAX_DESIRED / EM_BLOCK_SIZE;

    int groupBlocks = 8; // for 4096 images
    int extraBlocks = 2;
    int delta = (groupBlocks + extraBlocks) * EM_BLOCK_SIZE;

    cilk_for (int xblock = x_block_min; xblock <= x_block_max; xblock += groupBlocks)
    {
        cilk_for (int yblock = y_block_min; yblock <= y_block_max; yblock += groupBlocks)
        {
            cv::Mat siftBlock(delta, delta, CV_8UC1, cv::Scalar(0));
            //cout << "made it, sift type " << siftBlock.type() << "\n";
            for (int x = xblock; x < min(xblock + groupBlocks + extraBlocks, x_block_max); x++)
            {
                for (int y = yblock; y < min(yblock + groupBlocks + extraBlocks, y_block_max); y++)
                {
                    string filename = getEMImageName(z, y, x);
                    cv::Mat emBlock = cv::imread(filename, cv::IMREAD_GRAYSCALE);
                    //cout << "emblock type: " << emBlock.type() << "\n";

                    cv::Range colRange((x - xblock) * EM_BLOCK_SIZE, 
                                    (x - xblock + 1) * EM_BLOCK_SIZE);
                    cv::Range rowRange((y - yblock) * EM_BLOCK_SIZE, 
                                    (y - yblock + 1) * EM_BLOCK_SIZE);
                    cv::Mat siftSection = siftBlock(rowRange, colRange);
                    //cout << "siftSection type: " << siftSection.type();
                    emBlock.copyTo(siftSection);
                    //siftSection.setTo(emBlock);
                }
            }
            if (cv::countNonZero(siftBlock) == 0) continue;
            //cout << "all the way\n";
            int begin_x_i = xblock * EM_BLOCK_SIZE;
            int begin_y_i = yblock * EM_BLOCK_SIZE;
            string siftName = getSiftImageName(z, begin_y_i, begin_x_i, delta);
            cv::imwrite(siftName, siftBlock);
        }
    }
}

string getSiftImageName(int z, int y_i, int x_i, int delta)
{
    string filename = to_string(x_i)
        + "_" + to_string(y_i)
        + "_" + to_string(delta)
        + "_" + to_string(delta)
        + "_" + to_string(z)
        + ".png";
    return SIFT_PATH + to_string(z) + "/" + filename;
}

uint8_t** openEMImages(int z)
{
    uint8_t** array = new uint8_t*[X_I_MAX];
    for (int x = 0; x < X_I_MAX; x++)
    {
        array[x] = new uint8_t[Y_I_MAX];
    }
    cilk_for (int x = 0; x < X_I_MAX; x++)
    {
        for (int y = 0; y < Y_I_MAX; y++)
        {
            array[x][y] = 0;
        }
    }

    cilk_for (int xem = X_MIN_DESIRED / EM_BLOCK_SIZE; xem < X_MAX_DESIRED / EM_BLOCK_SIZE + 1; xem++)
    {
        for (int yem = Y_MIN_DESIRED / EM_BLOCK_SIZE; yem < Y_MAX_DESIRED / EM_BLOCK_SIZE + 1; yem++)
        {
            string filePath = getEMImageName(z, yem, xem);
            ifstream f(filePath.c_str());
            if (!f.good()) continue;

            cimg_library::CImg<short> image(filePath.c_str());
            for (int xind = 0; xind < EM_BLOCK_SIZE; xind++)
            {
                int x_i = xem * EM_BLOCK_SIZE + xind;
                for (int yind = 0; yind < EM_BLOCK_SIZE; yind++)
                {
                    int y_i = yem * EM_BLOCK_SIZE + yind;
                    array[x_i][y_i] = image(xind, yind);
                }
            }
        }
    }
    return array;
}



