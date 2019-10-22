#include <iostream>

#include "dem_parser/dem_parser.h"
#include "echo_sim/echo_sim.h"
#include "cxxopts.h"
#include "options.h"

void printHelp();
//void printHelpBig();

using std::cout;
using std::endl;

cxxopts::ParseResult parse(int argc, char* argv[]) {
    try {
        cxxopts::Options options(argv[0], " ");
        options.add_options()
            ("t,lat", "Latitude (degrees)", cxxopts::value<float>())
            ("n,lon", "Longitude (degrees)", cxxopts::value<float>())
            ("r,radius", "Radius (meters)", cxxopts::value<float>())
            ("disable-elevation", "Disable Elevation Reader", cxxopts::value<bool>()->default_value("false"))           
            ("h,help", "Print help page");
        return options.parse(argc, argv);
    } catch (cxxopts::OptionException& e) {
        std::cout << "Error parsing options: " << e.what() << endl << "Usage:" << endl;
        printHelp();
        exit(1);
    }
}

int main(int argc, char*argv[]) {
    auto result = parse(argc, argv);
    options_t O;
    if (!result.count("help")) {
        if (result.count("lat"))
            O.SIMULATOR_ORIGIN_LAT = result["lat"].as<float>(); 
        if (result.count("lon"))
            O.SIMULATOR_ORIGIN_LON = result["lon"].as<float>(); 
        if (result.count("radius"))
            O.SIMULATOR_RADIUS = result["radius"].as<float>();
        if (result.count("disable-elevation"))
            O.DEM_PARSER_DISABLE_ELEVATION = 1;
    } else {
        printHelp();
        return 1; 
    }
    EchoSimulator Simulator(&O);
    Simulator.PopulateAttenTable();
    Simulator.SaveToFile("output.atten");


}

void printHelp(){
    cout << "clutter_sim \n\t--lat [Latitude Angle] \n\t--lon [Longitude Angle] \n\t--radius [Simulation Radius] \n\t-o [Output File]" << endl;
}
