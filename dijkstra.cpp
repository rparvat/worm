#include <iostream>
#include <string>
#include "dijkstra.h"

void computeNeurons(int z)
{
    Graph graph = openProbs(z);
    auto seeds = getSeeds(z);
}

Graph openProbs(int z)
{
    return Graph(z);
}

// returns map from seed id to (x,y) coordinates of the seed in this frame.
map<int, pair<int, int>>* getSeeds(int z)
{ 
    auto seeds = new map<int, pair<int, int>>();
    return seeds;
}


/*
cimg::CImg<unsigned char> openMembraneIndiv(int z, int y, int x) {
    string z_str = locToString(z);
    string y_str = locToString(y);
    string x_str = locToString(z);
    
    string filename = z_str + "_" + y_str + "_" + x_str + ".jpg";
    cimg::CImg<unsigned char> membrane(filename.c_str());
    //membrane.get_crop
    return membrane;
}
*/


