#include <string>

using namespace std;

class Graph {
    float** halfProbs;
    int x_max, y_max;
    
  public:
    float getEdgeWeight(int x1, int y1, int x2, int y2);
    Graph(int z);
    ~Graph();
};
        

