#include <iostream>
#include <string>
#include "dijkstra.h"
#include "dijkstra_thread.h"
#include <fstream>
#include <cilk/cilk.h>
#include <climits>
#include <cstdlib>
#include <unordered_set>
#include <opencv2/opencv.hpp>

float DEFAULT_DISTANCE = INT_MAX;
int DEFAULT_SEED = 0;
int MAX_DISTANCE = 128;
bool PARALLEL_WRITE = false;
string SEED_LOCATION = "/home/rajeev/worm/skeleton/new_seeds.txt";

static map<Point, int> radii;

// returns map from seed id to (x,y) coordinates of the seed in this frame.
map<int, vector<Point>>* getSeeds(int desiredZ)
{ 
    // assuming the input is ID X Y Z
    // and top of input has number of things
    cout << "getting seeds... ";
    string filename = SEED_LOCATION;
    ifstream in(filename, ios_base::in);
    
    auto seeds = new map<int, vector<pair<int, int>>>();
    int numSeeds;

    in >> numSeeds;

    int count = 0;
    for (int i = 0; i < numSeeds; i++)
    {
        int seed, x, y, z, radius;
        in >> seed;
        in >> x;
        in >> y;
        in >> z;
        in >> radius;

        pair<int, int> tup(x, y);
        if (desiredZ == z || desiredZ == -1)
        {
            (*seeds)[seed].push_back(tup);
            count++;
        }
        if (desiredZ == z)
        {
            radii[tup] = radius;
        }
    }
    cout << "done! There are " << count << " seeds";
    if (desiredZ != -1) cout << " for this z.";
    cout << "\n";
    return seeds;
}


Dijkstra::Dijkstra(Graph& inputGraph): 
    graph(inputGraph)
{
    threadsDone = 0;
    initArrays();
}

void Dijkstra::initArrays()
{
    output = new double*[graph.x_max];

    for (int i = 0; i < graph.x_max; i++)
    {
        output[i] = new double[graph.y_max];
    }
    double defaultCombined = combineDistSeed(DEFAULT_DISTANCE, DEFAULT_SEED);
    cilk_for (int i = 0; i < graph.x_max; i++)
    {
        for (int j = 0; j < graph.y_max; j++)
        {
            output[i][j] = defaultCombined;
        }
    }
}

void Dijkstra::saveSeeds()
{
    // map seeds to pixel values
    auto seeds = getSeeds(-1);
    map<int, cv::Vec3b> seedMap;
    unordered_set<uint64_t> colorSet;

    seedMap[DEFAULT_SEED] = cv::Vec3b(0, 0, 0);

    string seedOutputPath = OUTPUT_PATH + to_string(graph.z) + "/";
    string colorPath = seedOutputPath;
    ofstream colorfile(colorPath + "output_colors.txt");
    colorfile << "seed r g b\n";
    srand(1);
    // only using the cell ids for this, so don't need to
    //      iterate over the vectors
    for (auto each : *seeds)
    {
        cv::Vec3b colorVec;
        uint64_t red, green, blue, 
                 combined;
        while (true)
        {
            red = rand() % 256;
            green = rand() % 256;
            blue = rand() % 256;
            colorVec = cv::Vec3b(red, green, blue);
            combined = 1000 * 1000 * red + 1000 * green + blue;
            if (!colorSet.count(combined)) break;
        }
        seedMap[each.first] = colorVec;
        colorSet.insert(combined);
        colorfile << each.first << " " << red << " " << green << " " << blue << "\n";
    }
    colorfile.close();

    cout << "saving seed assignments...";
    cout.flush();
    cv::Mat& img = *new cv::Mat(
            graph.desired_y_max - graph.y_min, 
            graph.desired_x_max - graph.x_min, 
            CV_8UC3,
            seedMap[DEFAULT_SEED]);
    cilk_for (int x = graph.x_min; x < graph.desired_x_max; x++)
    {
        for (int y = graph.y_min; y < graph.desired_y_max; y++)
        {
            img.at<cv::Vec3b>(cv::Point(x - graph.x_min, y - graph.y_min)) 
                = seedMap[separateDistSeed(output[x][y]).second];
        }
    }

    if (SHOW_SEEDS)
    {
        auto newSeeds = getSeeds(graph.z);
        auto allPoints = condenseSeeds(newSeeds);

        cv::Vec3b seedVec = cv::Vec3b(255, 255, 255);
        for (auto point : allPoints)
        {
            auto x = point.first;
            auto y = point.second;
            auto radius = radii[point];
            for (auto i = max(graph.x_min, x - radius); 
                    i <= min(x + radius, graph.desired_x_max - 1);
                    i++)
            {
                for (auto j = max(graph.y_min, y - radius); 
                        j <= min(y + radius, graph.desired_y_max - 1); 
                        j++)
                {
                    if (sqrt((i-x)*(i-x) + (j-y)*(j-y)) <= radius)
                    {
                        img.at<cv::Vec3b>(cv::Point(i - graph.x_min, j - graph.y_min))
                            = seedVec;
                    }
                }
            }
        }
        delete(newSeeds);
    }
    if (PARALLEL_WRITE)
    {
        int min_x_block = graph.x_min / BLOCK_SIZE;
        int max_x_block = graph.desired_x_max / BLOCK_SIZE - 1;

        int min_y_block = graph.y_min / BLOCK_SIZE;
        int max_y_block = graph.desired_y_max / BLOCK_SIZE - 1;
        cilk_for (int xblock = min_x_block; xblock <= max_x_block; xblock++)
        {
            for (int yblock = min_y_block; yblock <= max_y_block; yblock++)
            {
                cv::Range colRange(xblock * BLOCK_SIZE, (xblock + 1) * BLOCK_SIZE);
                cv::Range rowRange(yblock * BLOCK_SIZE, (yblock + 1) * BLOCK_SIZE);
                cv::Mat blockImg = img(rowRange, colRange);

                string imageName = to_string(xblock) + "_"
                    + to_string(yblock) + ".tiff";
                cv::imwrite(seedOutputPath + imageName, blockImg);
            }
        }
    }
    else
    {
        cv::imwrite(seedOutputPath + "output_" + to_string(graph.z) + ".tiff", img);
    }
    cout << " done with seed image!\n";
    cout.flush();
    delete(&img);
    delete(seeds);
}

void Dijkstra::saveDists()
{
    // now save an image giving distances
    //
    cout << "saving dists image...";
    cout.flush();

    string distsOutputPath = OUTPUT_PATH + to_string(graph.z) + "/";
    cv::Mat& dists = *new cv::Mat(
            graph.desired_y_max - graph.y_min, 
            graph.desired_x_max - graph.x_min, 
            CV_8UC1,
            cv::Scalar(0));
    cilk_for (int x = graph.x_min; x < graph.desired_x_max; x++)
    {
        for (int y = graph.y_min; y < graph.desired_y_max; y++)
        {
            float dist = separateDistSeed(output[x][y]).first;
            uint8_t toSave = (dist == DEFAULT_DISTANCE) ? 0 : 
                min(int(dist), 255);
            dists.at<uint8_t>(cv::Point(x - graph.x_min, y - graph.y_min)) 
                = toSave;
        }
    }
    cv::imwrite(distsOutputPath + "output_" + to_string(graph.z) + "_dists.tiff", dists);
    cout << " done!\n";
    cout.flush();
    delete(&dists);
}

void saveProbs(int z, int blur)
{
    // now save an image giving distances
    //
    Graph& graph = *Graph::getNewGraph(z, 1, blur);
    cout << "saving probs image...";
    cout.flush();
    cv::Mat& probs = *new cv::Mat(
            graph.desired_y_max - graph.y_min, 
            graph.desired_x_max - graph.x_min, 
            CV_8UC1,
            cv::Scalar(0));
    cilk_for (int x = graph.x_min; x < graph.desired_x_max; x++)
    {
        for (int y = graph.y_min; y < graph.desired_y_max; y++)
        {
            uint8_t toSave = (graph.halfProbs[x][y] == DEFAULT_PROBABILITY) ? 0 : 
                int(roundf(graph.halfProbs[x][y] * 256));
            probs.at<uint8_t>(cv::Point(x - graph.x_min, y - graph.y_min)) 
                = toSave;
        }
    }
    if (SHOW_SEEDS)
    {
        auto newSeeds = getSeeds(graph.z);
        auto allPoints = condenseSeeds(newSeeds);

        uint8_t seedInt = 255;
        for (auto point : allPoints)
        {
            auto x = point.first;
            auto y = point.second;
            auto radius = radii[point];
            for (auto i = max(graph.x_min, x - radius); 
                    i <= min(x + radius, graph.desired_x_max - 1);
                    i++)
            {
                for (auto j = max(graph.y_min, y - radius); 
                        j <= min(y + radius, graph.desired_y_max - 1); 
                        j++)
                {
                    if (sqrt((i-x)*(i-x) + (j-y)*(j-y)) <= radius)
                    {
                        probs.at<uint8_t>(cv::Point(i - graph.x_min, j - graph.y_min))
                            = seedInt;
                    }

                }
            }
        }
        delete(newSeeds);
    }
    cv::imwrite(OUTPUT_PATH + "output_" + to_string(graph.z) + "_probs.png", probs);
    cout << " done!\n";
    cout.flush();
    delete(&probs);
}


void saveEM(int z)
{
    cout << "saving EM image now... ";
    uint8_t** em_vals = openEMImages(z);
    cv::Mat& em = *new cv::Mat(
            Y_MAX_DESIRED - Y_MIN_DESIRED,
            X_MAX_DESIRED - X_MIN_DESIRED,
            CV_8UC1,
            cv::Scalar(0));
    cilk_for (int x = X_MIN_DESIRED; x < X_MAX_DESIRED; x++)
    {
        for (int y = Y_MIN_DESIRED; y < Y_MAX_DESIRED; y++)
        {
            em.at<uint8_t>(cv::Point(x - X_MIN_DESIRED, y - Y_MIN_DESIRED)) 
                = em_vals[x][y];
        }
    }

    cv::imwrite(OUTPUT_PATH + "output_" + to_string(z) + "_em.png", em);
    for (auto i = 0; i < X_MAX_DESIRED; i++)
    {
        delete(em_vals[i]);
    }
    delete(em_vals);
    delete(&em);
    cout << "done!\n";

}

DijkstraThread::DijkstraThread(int seedNum, vector<Point> points, Dijkstra& original):
    dijkstra(original)
{
    seed = seedNum;
    for (auto loc : points)
    {
        auto it = distances.emplace(float(0.0), loc);
        iterators[loc] = it;
    }
}

double combineDistSeed(float distance, int seed)
{
    double combined;
    float* floatCombined = (float*) &combined;
    *floatCombined = distance;
    *(int*)(floatCombined + 1) = seed;
    return combined;
}

pair<float, int> separateDistSeed(double combined)
{
    float* floatCombined = (float*) &combined;
    float distance = *floatCombined;
    int seed = *(int*)(floatCombined + 1);
    return pair<float, int>(distance, seed);
}

void DijkstraThread::run()
{
    int updateCount = 0;
    auto& graph = this->dijkstra.graph;
    while (distances.size() > 0)
    {
        auto it = distances.begin();

        float distance = it->first;
        Point point = it->second;
        
        distances.erase(it);
        iterators.erase(point);

        double desiredCombined = combineDistSeed(distance, seed);
        double curCombined = this->dijkstra.output[point.first][point.second];
        float curFinal = separateDistSeed(curCombined).first;
        if (curFinal <= distance) continue;

        bool updated = false;
        while (curFinal > distance)
        {
            uint64_t finalUint = __sync_val_compare_and_swap(
                (uint64_t*) &(this->dijkstra.output[point.first][point.second]),
                *(uint64_t*)(&curCombined),
                *(uint64_t*)(&desiredCombined));
            double newCombined = *(double*)(&finalUint);
            if (newCombined == curCombined)
            {
                updated = true;
                break;
            }
            else
            {
                curFinal = separateDistSeed(newCombined).first;
                curCombined = newCombined;
            }
        }

        if (!updated) continue;
        for (Point neighbor: graph.getNeighbors(point))

        {
            float curFinalDist = separateDistSeed(this->dijkstra.output[neighbor.first][neighbor.second]).first;
            float newDistance = distance + graph.getEdgeWeight(point, neighbor);
            if (newDistance > MAX_DISTANCE) continue;

            bool shouldUpdate = (newDistance < curFinalDist);
            if (shouldUpdate && iterators.count(neighbor))
            {
                it = iterators[neighbor];
                if (newDistance < it->first)
                {
                    distances.erase(it);
                    iterators.erase(neighbor);
                }
                else
                {
                    shouldUpdate = false;
                }
            }
            if (shouldUpdate) {
                it = distances.emplace(newDistance, neighbor);
                iterators[neighbor] = it;
            }
        }
    }
    dijkstra.threadsDone++;

    cout << "thread done #" << dijkstra.threadsDone << "\n";
    cout.flush();
}

Dijkstra::~Dijkstra()
{
    cilk_for (auto i = 0; i < graph.x_max; i++)
    {
        delete(output[i]);
    }
    delete(output);
}

void reconstruct(int z, bool saveSeeds, bool saveDists, int edgePower, int blur)
{
    auto seeds = getSeeds(z);

    // prepare the graph, and zero out areas around seeds
    Graph& graph = *Graph::getNewGraph(z, edgePower, blur);
    graph.zeroSeeds(*seeds, radii);

    // initialize and run dijsktra
    Dijkstra dijkstra(graph);
    cout << "Dijkstra is initialized\n";

    for (auto& mapPair : *seeds)
    {
        int seed = mapPair.first;
        auto thread = new DijkstraThread(seed, mapPair.second, dijkstra);
        cilk_spawn thread->run();
    }
    cilk_sync;
    if (saveSeeds) dijkstra.saveSeeds();
    if (saveDists) dijkstra.saveDists();
}


