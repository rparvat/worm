#include <boost/program_options.hpp>
#include "dijkstra.h"
#include <iostream>
#include <iterator>

namespace po = boost::program_options;
using namespace std;

int main(int ac, char* av[]) 
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("z", po::value<int>(), "(required) desired z slice to analyze")
            ("edgePower", po::value<int>(), "power probabilities are taken to; -1 indicates log")
            ("saveSeeds", po::value<bool>(), "whether to save seed image")
            ("saveDists", po::value<bool>(), "whether to save distance image")
            ("maxDistance", po::value<int>(), "maximum Dijkstra distance allowed")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }

        int z;
        if (vm.count("z"))
        {
            z = vm["z"].as<int>();
            cout << "Computing z slice " << z << "\n";
        }
        else
        {
            cout << "must provide z!\n";
            return -1;
        }


        int edgePower = 1;
        if (vm.count("edgePower")) {
            edgePower = vm["edgePower"].as<int>();
            cout << "Edge probability power was set to " 
                << edgePower << ".\n";
        } else {
            if (edgePower == -1)
            {
                cout << "Default log probability weightage used.\n";
            }
            else
            {
                cout << "Default edge probability power of " << edgePower << " used.\n";
            }
        }

        bool saveSeeds = true;
        if (vm.count("saveSeeds"))
        {
            saveSeeds = vm["saveSeeds"].as<bool>();
            cout << (saveSeeds ? "" : "not ") << "saving seed image\n";
        }
        else
        {
            cout << "By default " << (saveSeeds ? "" : "not ") << "saving seed image\n";;
        }

        bool saveDists = false;
        if (vm.count("saveDists"))
        {
            saveDists = vm["saveDists"].as<bool>();
            cout << (saveDists ? "" : "not ") << "saving distance image\n";
        }
        else
        {
            cout << "By default " << (saveDists ? "" : "not ") << "saving distance image\n";;
        }

        if (vm.count("maxDistance"))
        {
            MAX_DISTANCE = vm["maxDistance"].as<int>();
        }

        reconstruct(z, saveSeeds, saveDists, edgePower);
    }
    catch (exception& e)
    {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...) 
    {
        cout << "unknown error :(\n";
    }
    return 0;
}
