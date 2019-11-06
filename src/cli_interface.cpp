#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono;

#include "dem_parser/dem_parser.h"
#include "echo_sim/echo_sim.h"
#include "cxxopts.h"
#include "options.h"

using std::cout;
using std::endl;

cxxopts::ParseResult parse(int argc, char* argv[],cxxopts::Options* options ) {
    try {
        options->add_options()
            ("t,lat", "Latitude (degrees)", cxxopts::value<float>())
            ("n,lon", "Longitude (degrees)", cxxopts::value<float>())
            ("r,radius", "Radius (meters)", cxxopts::value<float>())
            ("v,verbose", "Verbose mode", cxxopts::value<bool>()->default_value("false"))
            ("disable-elevation", "Disable elevation reader", cxxopts::value<bool>()->default_value("false"))
            ("export-azimuth", "Export Azimuth Angles", cxxopts::value<bool>()->default_value("false"))
            ("export-elevation", "Export Elevation Angles", cxxopts::value<bool>()->default_value("false"))  
            ("export-elevation-map", "Export Elevation Map", cxxopts::value<bool>()->default_value("false"))   
            ("export-grazing", "Export Grazing Angles", cxxopts::value<bool>()->default_value("false"))   
            ("export-shadowing", "Export Shadowing", cxxopts::value<bool>()->default_value("false"))
            
            ("frequency", "The frequency of the carrier wave. (Hz)", cxxopts::value<float>()->default_value("200000000"))
            ("erp", "The Effective Radiated Power (ERP). (W)", cxxopts::value<float>()->default_value("400000"))
            ("pulse-length", "The length of the transmitted pulse. (s)", cxxopts::value<float>()->default_value("43.333333E-6"))
            ("range-bin-count", "The number of range bins.", cxxopts::value<float>()->default_value("540"))
            ("range-bin-period", "The period of each range bin. (s)", cxxopts::value<float>()->default_value("3.333333E-6"))
            ("azimuth-angle-count", "The number of azimuth angle bins.", cxxopts::value<float>()->default_value("4096"))
            ("wave-speed", "The speed that the wave propogates. (m/s)", cxxopts::value<float>()->default_value("299702505.269398111"))
            
            ("srtm", "SRTM folder", cxxopts::value<std::string>())
            ("antenna-file", "Antenna pattern file", cxxopts::value<std::string>())
            ("threads", "Number of CPU threads", cxxopts::value<int>())
            ("o,output", "Output file name", cxxopts::value<std::string>()->default_value("output.atten"))
            ("h,help", "Print help page");
        return options->parse(argc, argv);
    } catch (cxxopts::OptionException& e) {
        std::cout << "Error parsing options: " << e.what() << endl << options->help() <<endl;
        
        exit(1);
    }
}

int main(int argc, char*argv[]) {
    cxxopts::Options* options = new cxxopts::Options(argv[0], " ");;
    auto result = parse(argc, argv, options);
    
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
        if (result.count("output")) 
            O.SIMULATOR_OUTPUT_FILENAME = result["output"].as<std::string>();
        if (result.count("threads"))
            O.SIMULATOR_THREAD_COUNT = result["threads"].as<int>();
        else
            O.SIMULATOR_THREAD_COUNT = std::thread::hardware_concurrency();
        if (result.count("wave-speed"))
            O.SIMULATOR_WAVE_SPEED = result["wave-speed"].as<float>();
        if (result.count("frequency"))
            O.SIMULATOR_TRANSMIT_FREQUENCY = result["frequency"].as<float>();
        if (result.count("erp"))
            O.SIMULATOR_TRANSMIT_POWER = result["erp"].as<float>();
        if (result.count("antenna-file"))
            O.SIMULATOR_ANTENNA_FILENAME = result["antenna-file"].as<std::string>();
    } else {
        cout << options->help();
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

