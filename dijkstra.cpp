#include "dijkstra.h"

string locToString(int loc)
{
    int DEC_SIZE = 8;
    string s = to_string(loc);
    for (auto i = 0; i < DEC_SIZE - s.length(); i++)
    {
        s = "0" + s;
    }
    return s;
}

cimg::CImg<unsigned char> openMembrane(int z, int y, int x) {
    string z_str = locToString(z);
    string y_str = locToString(y);
    string x_str = locToString(z);
    
    string filename = z_str + "_" + y_str + "_" + x_str + ".jpg";
    cimg::CImg<unsigned char> membrane(filename.c_str());
    //membrane.get_crop
    return membrane;
}
