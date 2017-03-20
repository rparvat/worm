#include <iostream>
#include <string>
#include "dijkstra.h"
#include <fstream>

// returns map from seed id to (x,y) coordinates of the seed in this frame.
map<int, pair<int, int>>* getSeeds(int desiredZ)
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


void reconstruct(int z)
{
    Graph graph(z);
    auto seeds = getSeeds(z);
}


