#include "dem_parser/dem_parser.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ofstream;
using std::ios;

/** ElevationReader::exportMap
 * DESCRIPTION:
 *      Saves the map to a file.
 * ARGUMENTS:
 *      const char* filename
 *          The name of the file to write to.
 */
void ElevationMap::exportMap() {
    uint8_t fileVersion = 0x10;

    /* Export for Elevation Map */
    if (Options->DEM_PARSER_EXPORT_ELEVATION_MAP) {
        ofstream elevationMapExport("elevation_map.bin", ios::out | ios::binary);
        if (elevationMapExport.is_open()) {
            elevationMapExport.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
            elevationMapExport.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
            elevationMapExport.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
            for (int i = 0; i < mapSizeX; i++)
                for (int j = 0; j < mapSizeY; j++)
                    elevationMapExport.write(   reinterpret_cast<char*>(&elevation_map[i][j]),
                                                sizeof(elevation_map[i][j])
                                            );
            elevationMapExport.close();
            if (Options->PROG_VERBOSE)
                cout << "  Elevation map exported sucessfully to elevation_map.bin" << endl;
        }
        
    }
    
    /* Export for Grazing Angle */
    if (Options->DEM_PARSER_EXPORT_GRAZING_ANGLE) {
        ofstream grazingAngleExport("grazing_angle.bin", ios::out | ios::binary);
        if (grazingAngleExport.is_open()) {
            grazingAngleExport.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
            grazingAngleExport.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
            grazingAngleExport.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
            for (int i = 0; i < mapSizeX; i++)
                for (int j = 0; j < mapSizeY; j++)
                    grazingAngleExport.write(   reinterpret_cast<char*>(&map[i][j].grazing),
                                                sizeof(map[i][j].grazing)
                                            );
            grazingAngleExport.close();
            if (Options->PROG_VERBOSE)
                cout << "  Grazing angles exported sucessfully to grazing_angle.bin" << endl;
        }
        
    }
    
    /* Export for Azimuth Angle */
    if (Options->DEM_PARSER_EXPORT_AZIMUTH_ANGLE) {
        ofstream azAngleExport("azimuth_angle.bin", ios::out | ios::binary);
        if (azAngleExport.is_open()) {
            azAngleExport.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
            azAngleExport.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
            azAngleExport.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
            for (int i = 0; i < mapSizeX; i++)
                for (int j = 0; j < mapSizeY; j++)
                    azAngleExport.write(   reinterpret_cast<char*>(&map[i][j].az),
                                                sizeof(map[i][j].az)
                                            );
            azAngleExport.close();
            if (Options->PROG_VERBOSE)
                cout << "  Azimuth angles exported sucessfully to azimuth_angle.bin" << endl;
        }
        
    }
    
    /* Export for Elevation Angle */
    if (Options->DEM_PARSER_EXPORT_ELEVATION_ANGLE) {
        ofstream elAngleExport("elevation_angle.bin", ios::out | ios::binary);
        if (elAngleExport.is_open()) {
            elAngleExport.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
            elAngleExport.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
            elAngleExport.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
            for (int i = 0; i < mapSizeX; i++)
                for (int j = 0; j < mapSizeY; j++)
                    elAngleExport.write(   reinterpret_cast<char*>(&map[i][j].el),
                                                sizeof(map[i][j].el)
                                            );
            elAngleExport.close();
            if (Options->PROG_VERBOSE)
                cout << "  Elevation angles exported sucessfully to elevation_angle.bin" << endl;
        }
        
    }
    
    /* Export for Shadowing*/
    if (Options->DEM_PARSER_EXPORT_SHADOWING && Options->SIMULATOR_SHADOWING_ENABLED) {
        ofstream shadowingExport("shadowing.bin", ios::out | ios::binary);
        if (shadowingExport.is_open()) {
            shadowingExport.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
            shadowingExport.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
            shadowingExport.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
            for (int i = 0; i < mapSizeX; i++)
                for (int j = 0; j < mapSizeY; j++)
                    shadowingExport.write(   reinterpret_cast<char*>(&map[i][j].az),
                                                sizeof(map[i][j].az)
                                            );
            shadowingExport.close();
            if (Options->PROG_VERBOSE)
                cout << "  Shadowing exported sucessfully to shadowing.bin" << endl;
        }
    }
}
