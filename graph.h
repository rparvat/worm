#include <string>
#include <vector>

using namespace std;

typedef pair<int, int> Point;

class Graph {
    float** halfProbs;
    int x_max, y_max;
    
  public:
    float getEdgeWeight(int x1, int y1, int x2, int y2);
    vector<pair<int, int>> getNeighbors(int x, int y);
    Graph(int z);
    ~Graph();
};
        

