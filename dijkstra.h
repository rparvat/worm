#include <iostream>
#include <string>
#include "CImg.h"

using namespace std;

string locToString(int loc)
{
    int DEC_SIZE = 8
    string s = to_string(loc);
    for (auto i = 0; i < DEC_SIZE - s.length(); i++)
    {
        s = "0" + s;
    return s;
}

// opens the 
CImg<uint8_t> openMembrane(z, y, x) {
    string z_str = locToString(z);
    string y_str = locToString(y);
    string x_str = locToString(z);
    
    string filename = z_str + "_" + y_str + "_" + x_str + ".jpg";
    CImg<uint8_t> membrane(filename);
    //membrane.get_crop
    return membrane;
}
