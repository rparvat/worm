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
            ("blur", po::value<int>(), "size of blurring window used on probability maps (n x n window); 0 means do not blur")
            ("saveProbs", po::value<bool>(), "whether to save new probability image")
            ("skipRecon", po::value<bool>(), "flag to skip seed reconstruction")
            ("saveEM", po::value<bool>(), "whether to save EM image")
            ("showSeeds", po::value<bool>(), "to show seeds in probs/seeds/em images")
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
        }
        if (edgePower == -1)
        {
            cout << "Log probability weightage used.\n";
        }
        else
        {
            cout << "Edge probability power of " << edgePower << " used.\n";
        }

        bool saveSeeds = true;
        if (vm.count("saveSeeds"))
        {
            saveSeeds = vm["saveSeeds"].as<bool>();
        }
        cout << (saveSeeds ? "" : "not ") << "saving seed image\n";

        bool saveDists = false;
        if (vm.count("saveDists"))
        {
            saveDists = vm["saveDists"].as<bool>();
        }
        cout << (saveDists ? "" : "not ") << "saving distance image\n";

        int blur = 0;
        if (vm.count("blur"))
        {
            blur = vm["blur"].as<int>();
        }
        if (blur)
        {
            cout << "blurring with window size " << blur << "\n";
        }
        else
        {
            cout << "not blurring\n";
        }

        if (vm.count("showSeeds")) SHOW_SEEDS = vm["showSeeds"].as<bool>();
        if (SHOW_SEEDS) cout << "showing seed localities!\n";

        if (vm.count("maxDistance"))
        {
            MAX_DISTANCE = vm["maxDistance"].as<int>();
        }
        cout << "max distance is " << MAX_DISTANCE << "\n";

        if (!vm.count("skipRecon") || !vm["skipRecon"].as<bool>()) 
        {
            reconstruct(z, saveSeeds, saveDists, edgePower, blur);
        }
        else
        {
            cout << "skipping recontruction\n";
        }

        if (vm.count("saveProbs") && vm["saveProbs"].as<bool>()) 
        {
            cout << "saving probs image\n";
            saveProbs(z, blur);
        }

        if (vm.count("saveEM") && vm["saveEM"].as<bool>())
        {
            cout << "saving EM image\n";
            saveEM(z);
        }

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
