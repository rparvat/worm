#pragma once

#include <unordered_map>
#include <map>
#include "graph.h"

using namespace std;

struct pairhash {
    public:
    template <typename T, typename U>
    std::size_t operator()(const pair<T, U> &x) const
    {
        return hash<T>()(x.first) ^ hash<U>()(x.second);
    }
};

class Dijkstra;

class DijkstraThread {
    public:
        multimap<float, Point> distances;
        unordered_map<Point, multimap<float, Point>::iterator, 
            pairhash> iterators;
        int seed;
        Dijkstra& dijkstra;

        DijkstraThread(int seedNum, vector<Point> points, Dijkstra& original);
        void run();
};


