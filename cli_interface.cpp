#include <iostream>

//#include "dem_parser/dem_parser.h"
//#include "echo_sim/echo_sim.h"
#include "cxxopts.h"

cxxopts::ParseResult parse (int argc, char* argv[]) {
    try {
        cxxopts::Options options(argv[0], " ");
        options.add_options()
            ("t,lat", "Latitude (degrees)", cxxopts::value<float>())
            ("n,lon", "Longitude (degrees)", cxxopts::value<float>())
            ("r,radius", "Radius (meters)", cxxopts::value<float>()) 
            ("h,help", "Print help page");
        return options.parse(argc, argv);
    } catch (cxxopts::OptionException& e) {
        std::cout << "Error parsing options: " << e.what() << std::endl;
        exit(1);
    }
}

int main(int argc, char*argv[]) {
    auto result = parse(argc, argv);
    if (!result.count("lat"))
        std::cout << "WTF";
    else
        std::cout << result["lat"].as<float>() << std::endl;
}
