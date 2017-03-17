#include <iostream>
#include <string>
#include "CImg.h"

namespace cimg = cimg_library;
using namespace std;

string locToString(int loc);
cimg::CImg<unsigned char> openMembrane(int z, int y, int x);
