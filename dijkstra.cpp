#include <iostream>
#include <string>
#include "dijkstra.h"
#include "dijkstra_thread.h"
#include <fstream>
#include <cilk/cilk.h>
#include <climits>
#include <cstdlib>
#include <opencv2/opencv.hpp>

float DEFAULT_DISTANCE = INT_MAX;
int DEFAULT_SEED = 0;
int MAX_DISTANCE = 128;
// returns map from seed id to (x,y) coordinates of the seed in this frame.
map<int, Point>* getSeeds(int desiredZ)
{ 
    // assuming the input is ID X Y Z
    // and top of input has number of things
    cout << "getting seeds... ";
    string filename = "/home/rajeev/worm/skeleton/seeds.txt";
    ifstream in(filename, ios_base::in);
    
    auto seeds = new map<int, pair<int, int>>();
    int numSeeds;

    in >> numSeeds;
    for (int i = 0; i < numSeeds; i++)
    {
        int seed, x, y, z;
        in >> seed;
        in >> x;
        in >> y;
        in >> z;
        if (desiredZ == z || desiredZ == -1)
        {
            pair<int, int> tup(x, y);
            seeds->insert(pair<int, Point>(seed, tup));
            //(*seeds)[seed] = tup;
        }
    }
    cout << "done!";
    if (desiredZ != -1) cout << " There are " << seeds->size() << " seeds for this z.";
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
    finalDists = new float*[graph.x_max];
    assignments = new int*[graph.x_max];

    for (int i = 0; i < graph.x_max; i++)
    {
        finalDists[i] = new float[graph.y_max];
        assignments[i] = new int[graph.y_max];
    }
    cilk_for (int i = 0; i < graph.x_max; i++)
    {
        for (int j = 0; j < graph.y_max; j++)
        {
            finalDists[i][j] = DEFAULT_DISTANCE;
            assignments[i][j]= DEFAULT_SEED;
        }
    }
}

void Dijkstra::saveSeeds()
{
    // map seeds to pixel values
    auto seeds = getSeeds(-1);
    map<int, cv::Vec3b> seedMap;
    seedMap[DEFAULT_SEED] = cv::Vec3b(0, 0, 0);

    ofstream colorfile("output_colors.txt");
    colorfile << "seed r g b\n";
    srand(1);
    for (auto each : *seeds)
    {
        uint64_t red = rand() % 256;
        uint64_t green = rand() % 256;
        uint64_t blue = rand() % 256;
        seedMap[each.first] = cv::Vec3b(red, green, blue);
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
                = seedMap[assignments[x][y]];
        }
    }
    cv::imwrite("output_" + to_string(graph.z) + ".png", img);
    cout << " done with seed image!\n";
    cout.flush();
    delete(&img);
}

void Dijkstra::saveDists()
{
    // now save an image giving distances
    //
    cout << "saving dists image...";
    cout.flush();
    cv::Mat& dists = *new cv::Mat(
            graph.desired_y_max - graph.y_min, 
            graph.desired_x_max - graph.x_min, 
            CV_8UC1,
            cv::Scalar(0));
    for (int x = graph.x_min; x < graph.desired_x_max; x++)
    {
        for (int y = graph.y_min; y < graph.desired_y_max; y++)
        {
            uint8_t toSave = (finalDists[x][y] == DEFAULT_DISTANCE) ? 0 : 
                min(int(finalDists[x][y]), 255);
            dists.at<uint8_t>(cv::Point(x - graph.x_min, y - graph.y_min)) 
                = toSave;
        }
    }
    cv::imwrite("output_" + to_string(graph.z) + "_dists.png", dists);
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
    for (int x = graph.x_min; x < graph.desired_x_max; x++)
    {
        for (int y = graph.y_min; y < graph.desired_y_max; y++)
        {
            uint8_t toSave = (graph.halfProbs[x][y] == DEFAULT_PROBABILITY) ? 0 : 
                int(roundf(graph.halfProbs[x][y] * 256));
            probs.at<uint8_t>(cv::Point(x - graph.x_min, y - graph.y_min)) 
                = toSave;
        }
    }
    cv::imwrite("output_" + to_string(graph.z) + "_probs.png", probs);
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
    cv::imwrite("output_" + to_string(z) + "_em.png", em);
    for (auto i = 0; i < X_MAX_DESIRED; i++)
    {
        delete(em_vals[i]);
    }
    delete(em_vals);
    delete(&em);
    cout << "done!\n";

}

DijkstraThread::DijkstraThread(int seedNum, Point loc, Dijkstra& original):
    dijkstra(original)
{
    seed = seedNum;

    auto it = distances.emplace(float(0.0), loc);
    iterators[loc] = it;
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

        float curFinal = this->dijkstra.finalDists[point.first][point.second];
        if (curFinal <= distance) continue;

        bool updated = false;
        while (curFinal > distance)
        {
            uint32_t finalUint = __sync_val_compare_and_swap(
                (uint32_t*) &(this->dijkstra.finalDists[point.first][point.second]),
                *(uint32_t*)(&curFinal),
                *(uint32_t*)(&distance));
            float newFinal = *(float*)(&finalUint);
            if (newFinal == curFinal)
            {
                //TODO: make this atomic!!
                this->dijkstra.assignments[point.first][point.second] = seed;
                updated = true;
                break;
            }
            else
            {
                curFinal = newFinal;
            }
        }

        if (!updated) continue;
        for (Point neighbor: graph.getNeighbors(point))

        {
            float curFinalDist = this->dijkstra.finalDists[neighbor.first][neighbor.second];
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
    for (auto i = 0; i < graph.x_max; i++)
    {
        delete(finalDists[i]);
        delete(assignments[i]);
    }
    delete(finalDists);
    delete(assignments);
}

void reconstruct(int z, bool saveSeeds, bool saveDists, int edgePower, int blur)
{
    auto seeds = getSeeds(z);

    // prepare the graph, and zero out areas around seeds
    Graph& graph = *Graph::getNewGraph(z, edgePower, blur);
    graph.zeroSeeds(*seeds);

    // initialize and run dijsktra
    Dijkstra dijkstra(graph);
    cout << "Dijkstra is initialized\n";

    for (auto& mapPair : *seeds)
    {
        int seed = mapPair.first;
        Point point = mapPair.second;
        auto thread = new DijkstraThread(seed, point, dijkstra);
        cilk_spawn thread->run();
    }
    cilk_sync;
    if (saveSeeds) dijkstra.saveSeeds();
    if (saveDists) dijkstra.saveDists();
}


