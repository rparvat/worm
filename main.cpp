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

    bool saveSeeds;
    int temp = 0;
    if (argc >= 3 && !sscanf(argv[2], "%d", &temp))
    {
        cout << "invalid saveSeeds\n";
        return -1;
    }
    saveSeeds = temp;

    bool saveDists;
    temp = 0;
    if (argc >= 4 && !sscanf(argv[3], "%d", &temp))
    {
        cout << "invalid saveDists\n";
        return -1;
    }
    saveDists = temp;


    cout << "reconstructing " << z;
    if (saveSeeds) cout << ", saving seeds";
    if (saveDists) cout << ", saving dists";
    cout << "!\n";

    reconstruct(z, saveSeeds, saveDists);
    //saveEM(z);
}
