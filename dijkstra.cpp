#include <iostream>
#include <string>
#include "dijkstra.h"
#include <fstream>
#include <unordered_map>

// returns map from seed id to (x,y) coordinates of the seed in this frame.
map<int, Point>* getSeeds(int desiredZ)
{ 
    // assuming the input is ID X Y Z
    // and top of input has number of things
    cout << "getting seeds... ";
    string filename = "home/rajeev/worm/skeleton/seeds.txt";
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
        x /= 2;
        y /= 2;
        z /= 30;
        if (desiredZ == z)
        {
            pair<int, int> tup(x, y);
            (*seeds)[seed] = tup;
        }
    }
    cout << "done!\n";
    return seeds;
}

struct pairhash {
    public:
    template <typename T, typename U>
    std::size_t operator()(const pair<T, U> &x) const
    {
        return hash<T>()(x.first) ^ hash<U>()(x.second);
    }
};

void reconstruct(int z)
{
    Graph graph(z);
    auto seeds = getSeeds(z);

    float finalDists[graph.x_max][graph.y_max];
    int assignments[graph.x_max][graph.y_max];

    for (int i = 0; i < graph.x_max; i++)
    {
        for (int j = 0; j < graph.y_max; j++)
        {
            finalDists[i][j] = 0;
            assignments[i][j]= 0;
        }
    }

    multimap<float, Point> distances;
    unordered_map<Point, multimap<float, Point>::iterator, pairhash> iterators;
    for (auto kv : *seeds)
    {
        int seed = kv.first;
        Point point = kv.second;
        auto it = distances.emplace(float(1.0), point);
        iterators[point] = it;
        assignments[point.first][point.second] = seed;
    }
    while (!distances.empty())
    {
        auto it = distances.begin();
        float distance = it->first;
        Point point = it->second;
        int seed = assignments[point.first][point.second];
        finalDists[point.first][point.second] = distance;

        distances.erase(it);
        for (Point neighbor: graph.getNeighbors(point))
        {
            if (finalDists[neighbor.first][neighbor.second] != 0) continue;
            float newDistance = distance + graph.getEdgeWeight(point, neighbor);
            bool shouldUpdate = true;
            if (iterators.count(neighbor))
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
                assignments[neighbor.first][neighbor.second] = seed;
                it = distances.emplace(newDistance, neighbor);
                iterators[point] = it;
            }
        }
    }
    // TODO: save this information somehow.                
}


