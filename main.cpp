#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "dijkstra.h"

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        cout << "must provide an argument!\n";
        return 0;
    }
    int z;
    if (!sscanf(argv[1], "%d", &z))
    {
        cout << "invalid z!\n";
        return -1;
    }
    cout << "reconstructing " << z << "!\n";
    reconstruct(z);
}
