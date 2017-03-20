#include "graph.h"
#include <map>

using namespace std;

//cimg::CImg<unsigned char> openMembraneIndiv(int z, int y, int x); 
map<int, Point>* getSeeds(int z);
void reconstruct(int z);
