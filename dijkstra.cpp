#include <iostream>
#include <string>
#include "dijkstra.h"
#include "dijkstra_thread.h"
#include <fstream>
#include <cilk/cilk.h>
#include <climits>

float DEFAULT_DISTANCE = INT_MAX;
int DEFAULT_SEED = 0;
int UPDATES_PER_RECONCILE = 1000;
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
        if (desiredZ == z)
        {
            pair<int, int> tup(x, y);
            seeds->insert(pair<int, Point>(seed, tup));
            //(*seeds)[seed] = tup;
        }
    }
    cout << "done! There are " << seeds->size() << " seeds.\n";
    return seeds;
}


Dijkstra::Dijkstra(Graph& inputGraph): 
    graph(inputGraph)
{
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

void Dijkstra::reconcile(DijkstraThread& thread)
{
    auto& distances = thread.distances;
    auto& iterators = thread.iterators;
    auto seed = thread.seed;
    auto& toUpdate = thread.toUpdate;

    mtx.lock();
    cout << "reconciling! there are " << toUpdate.size() << "updates\n";
    for (auto it = toUpdate.begin(); it != toUpdate.end(); it++)
    {
        float distance = it->second;
        Point point = it->first;
        
        float curFinalDist = finalDists[point.first][point.second];
        if (curFinalDist == DEFAULT_DISTANCE || distance < curFinalDist)
        {
            finalDists[point.first][point.second] = distance;
            assignments[point.first][point.second] = seed;
        }
    }
    toUpdate.clear();
    mtx.unlock();
}

void Dijkstra::save()
{
    //write to file
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

        toUpdate[point] = distance;

        distances.erase(it);
        iterators.erase(point);

        for (Point neighbor: graph.getNeighbors(point))
        {
            float curFinalDist = this->dijkstra.finalDists[neighbor.first][neighbor.second];
            float newDistance = distance + graph.getEdgeWeight(point, neighbor);

            bool shouldUpdate = (curFinalDist == DEFAULT_DISTANCE)
                || (newDistance < curFinalDist);
            shouldUpdate &= (!toUpdate.count(neighbor));

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
        updateCount++;
        if (updateCount == UPDATES_PER_RECONCILE)
        {
            dijkstra.reconcile(*this);
            updateCount = 0;
        }
    }
    cout << "thread done!\n";
}



void reconstruct(int z)
{
    Graph graph(z);
    auto seeds = getSeeds(z);

    Dijkstra dijkstra(graph);
    cout << "Dijkstra is initialized\n";

    for (auto& mapPair : *seeds)
    {
        int seed = mapPair.first;
        Point point = mapPair.second;
        cout << "spawning Dijkstra Thread!\n";
        auto thread = new DijkstraThread(seed, point, dijkstra);
        cilk_spawn thread->run();
    }
    cilk_sync;
    // TODO: save this information somehow.                
}


