#include <iostream>
#include <chrono>
using namespace std::chrono;

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
            ("v,verbose", "Verbose mode", cxxopts::value<bool>()->default_value("false"))
            ("disable-elevation", "Disable Elevation Reader", cxxopts::value<bool>()->default_value("false"))
            ("export-azimuth", "Export Azimuth Angles", cxxopts::value<bool>()->default_value("false"))
            ("export-elevation", "Export Elevation Angles", cxxopts::value<bool>()->default_value("false"))  
            ("export-elevation-map", "Export Elevation Map", cxxopts::value<bool>()->default_value("false"))   
            ("export-grazing", "Export Grazing Angles", cxxopts::value<bool>()->default_value("false"))   
            ("export-shadowing", "Export Shadowing", cxxopts::value<bool>()->default_value("false"))
            ("srtm", "SRTM folder", cxxopts::value<std::string>())
            ("threads", "Number of CPU threads", cxxopts::value<int>())           
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
        if (result.count("verbose"))
            O.PROG_VERBOSE = 1;
        if (result.count("export-azimuth"))
            O.DEM_PARSER_EXPORT_AZIMUTH_ANGLE = 1;
        if (result.count("export-elevation"))
            O.DEM_PARSER_EXPORT_ELEVATION_ANGLE = 1;
        if (result.count("export-elevation-map"))
            O.DEM_PARSER_EXPORT_ELEVATION_MAP = 1;
        if (result.count("export-grazing"))
            O.DEM_PARSER_EXPORT_GRAZING_ANGLE = 1;
        if (result.count("export-shadowing"))
            O.DEM_PARSER_EXPORT_SHADOWING = 1;
        if (result.count("srtm")) 
            O.DEM_PARSER_SRTM_FOLDER = result["srtm"].as<std::string>();
        if (result.count("threads"))
            O.SIMULATOR_THREAD_COUNT = result["threads"].as<int>();
    } else {
        printHelp();
        return 1; 
    }
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    EchoSimulator Simulator(&O);
    Simulator.PopulateAttenTable();
    Simulator.SaveToFile("output.atten");
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

    std::cout << "Execution time: " << time_span.count() << " seconds.";
    std::cout << std::endl;
}

void printHelp(){
    cout << "clutter_sim \n\t--lat [Latitude Angle] \n\t--lon [Longitude Angle] \n\t--radius [Simulation Radius] \n\t-o [Output File]" << endl;
}
