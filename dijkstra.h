#include "graph.h"
#include <map>

using namespace std;

//cimg::CImg<unsigned char> openMembraneIndiv(int z, int y, int x); 
Graph openProbs(int z);
map<int, pair<int, int>>* getSeeds(int z);
