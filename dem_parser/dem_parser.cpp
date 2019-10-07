#include "dem_parser.h"
#include <math.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

#include <thread>
#include <chrono>
#include <iostream>

#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))

/** ElevationReader::populateMap
 * DESCRIPTION:
 *      Populates the elevation map.
 * ARGUMENTS:
 *      double lat, lon
 *          The lattitude and longitude of the origin in degrees.
 *      double radius
 *          The radius from the origin to populate in meters.
 */
void ElevationMap::populateMap(double lat, double lon, double radius, double height){
    originLat = lat;
    originLon = lon;
    originHeight = ER.GetElevation(lat,lon) + height;
    
    threadCount = std::thread::hardware_concurrency();
    std::thread threads[threadCount];

    
    mapSizeX = 2 * radius / deltaDistance;
    mapSizeY = 2 * radius / deltaDistance;
    mapOriginX = mapSizeX/2;
    mapOriginY = mapSizeY/2;
    float mapDelta = float(mapSizeX-1)/float(threadCount);
    
    // Start allocating map.
    alloc_i = 0;
    threads[0] = std::thread(&ElevationMap::allocateMap,this);
    
    
    // Calculate origin and unit vectors
    origin = calculateECEF(originLat, originLon, originHeight);
   
    // y-axis reference
    axis[1] = calculateECEF(lat + 0.00001, lon, originHeight) - origin;
    // x-axis reference
    axis[0] = calculateECEF(lat, lon - 0.00001, originHeight) - origin;
    // z-axis = x cross y 
    axis[2] = axis[1].cross(axis[0]);
    
    for (int i = 0; i < 3; i++)
        axis[i] = axis[i].normalize();
    
    if (threadCount <= 1) {
        // Finish allocating map.
        threads[0].join();
        // Populate map using single thread.
        populatePartial(0, mapSizeX);
    }
    else {
        // Break map into N-1 parts.
        float mapDelta2 = float(mapSizeX-1)/float(threadCount - 1);
        
        // Use N-1 threads to populate elevation, azimuth, elevation, and radius.
        threads[1] = std::thread(&ElevationMap::populatePartial, this, 0, int(mapDelta2));
        for (int i = 2; i < threadCount; i++) 
            threads[i] = std::thread(   &ElevationMap::populatePartial,this, 
                                        int(mapDelta2*(i-1))+1,
                                        int(mapDelta2*(i))
                                    );
        // Wait for threads to complete.
        for (int i = 0; i < threadCount; i++)
            threads[i].join();
    }
        
    calculateShadowing();
    populateGrazingAngle();
    map[mapOriginX][mapOriginY].el = 0;
}

/** ElevationReader::calculateLatLon
 * DESCRIPTION:
 *      Calculates the latitude and longitude for a specific element on the map. 
 * ARGUMENTS:
 *      int x, y
 *          The element of the map. 
 *      float* lat, lon
 *          Pointers to the latitude and longitude. These will be overwritten. 
 */
void ElevationMap::calculateLatLon(int x, int y, float* lat, float* lon) {
    double latTemp = (double)originLat;
    double lonTemp = (double)originLon;

    double walkAzimuth = 0;
    if (x != mapOriginX) {
        if (x < mapOriginX)
            walkAzimuth = 90;
        else if (x > mapOriginX)
            walkAzimuth = 270;
        ER.WalkDistance(    (double)originLat, 
                            (double)originLon, 
                            walkAzimuth, 
                            (double)fabs(x-mapOriginX)*deltaDistance,
                            &latTemp, 
                            &lonTemp );
    }

    double latTemp2 = 0;
    double lonTemp2 = 0;
    walkAzimuth = 0;
    if (y != mapOriginY) {
        if (y < mapOriginY)
            walkAzimuth = 180;
        else if (y > mapOriginY)
            walkAzimuth = 0;
    }
    ER.WalkDistance (   (double)latTemp, 
                        (double)lonTemp, 
                        walkAzimuth,
                        (double)fabs(y-mapOriginY)*deltaDistance,
                        &latTemp2, 
                        &lonTemp2 );
    *lat = (float)latTemp2;
    *lon = (float)lonTemp2;
}

/** ElevationReader::calculateSphericalCoordinates
 * DESCRIPTION:
 *      Calculates the azimuth, elevation, and range from the origin. 
 * ARGUMENTS:
 *      int lat, lon
 *          The lattitude and longitude of the point.
 *      float* az, el, r
 *          Pointers to the azimuth angle, elevation angle, and range. 
 */
void ElevationMap::calculateSphericalCoordinates(float lat, float lon, float h, float* az, float* el, float* r) {
    ThreeVector local = calculateECEF(lat, lon, h);
    
    float x,y,z;
    x = (local - origin).dot(axis[0]);
    y = (local - origin).dot(axis[1]);
    z = (local - origin).dot(axis[2]);
    // Calculate radius, elevation angle, and azimuth angle relative to the transmitter.
    *r  = sqrt( pow(x,2) + 
                pow(y,2) + 
                pow(z,2) );
    *el = atan2(z, sqrt(pow(x,2) + pow(y,2)));
    *az = atan2(x, y);
}

/** ElevationReader::populateSphericalCoordinates
 * DESCRIPTION:
 *      Populates a partial section of the map with spherical coordinates.
 *      Used to multithread the process.
 * ARGUMENTS:
 *      int start, end
 *          The start and end points of the map. 
 */
void ElevationMap::populateSphericalCoordinates(int start, int end) {
    //for (int i = start; i <= end; i++)
        //for (int j = 0; j < mapSizeY; j++)
            //calculateSphericalCoordinates(i,j, &map[i][j].az, &map[i][j].el, &map[i][j].r);
}

void ElevationMap::populatePartial(int start, int end) {
    while (alloc_i < 1)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    for (int i = 0; i < mapSizeY; i++) {
        for (int j = start; j <= end; j++) {
            float lat, lon;
            calculateLatLon(i, j, &lat, &lon);
            // Load elevation of the point.
            elevation_map[i][j] = ER.GetElevation(lat, lon);
            // Calculate spherical coordinates.
            calculateSphericalCoordinates(  lat, 
                                            lon, 
                                            elevation_map[i][j], 
                                            &map[i][j].az, 
                                            &map[i][j].el, 
                                            &map[i][j].r);
        }
        // Wait for the next column to be allocated before continuing.
        while (i >= alloc_i - 1)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

/*  ElevationMap::allocateMap
    DESCRIPTION:
        Allocates the memory necessary for the map.
*/
void ElevationMap::allocateMap() {
    map = new chunk_t* [mapSizeX];
    elevation_map = new float* [mapSizeX];
    for (alloc_i = 0; alloc_i < mapSizeX; ) {
        map[alloc_i] = new chunk_t [mapSizeY];
        elevation_map[alloc_i] = new float [mapSizeY];
        alloc_i++;
    }
    alloc_i++;
}

/*  ElevationMap::deallocateMap
    DESCRIPTION:
        Allocates the memory necessary for the map.
*/
void ElevationMap::deallocateMap(){
    for (int i = 0; i < mapSizeX; i++)
        delete [] map[i];
    delete [] map;
}

/** ElevationReader::deallocateElevation
 * DESCRIPTION:
 *      Deallocates the elevation map.
 */
void ElevationMap::deallocateElevation() {
    for (int i = 0; i < mapSizeX; i++)
        delete [] elevation_map[i];
    delete [] elevation_map;
}


chunk_t ElevationMap::getMap(int x, int y) {
    return map[x][y];
}

/** ElevationReader::saveMap
 * DESCRIPTION:
 *      Saves the map to a file.
 * ARGUMENTS:
 *      const char* filename
 *          The name of the file to write to.
 */
void ElevationMap::saveMap(const char* filename) {
    uint8_t fileVersion = 0x10;
    ofstream fileOutput(filename, ios::out | ios::binary);
    fileOutput.write(reinterpret_cast<char*>(&fileVersion), sizeof(fileVersion));
    fileOutput.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
    fileOutput.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
    
    for (int i = 0; i < mapSizeX; i++)
        for (int j = 0; j < mapSizeY; j++)
            fileOutput.write(reinterpret_cast<char*>(&map[i][j]), sizeof(map[i][j]));
}

ElevationMap::~ElevationMap(){
    deallocateMap();
    deallocateElevation();
}

#ifdef DEBUG_DEM_PARSER

int main() {
    for (int i = 1; i < 2; i++) {
        auto start = std::chrono::steady_clock::now();
        ElevationMap E;
        E.populateMap(38.52, -98.10, 100*i,0);
        auto end = std::chrono::steady_clock::now();
        std::cout  << 100*i << "," << (end - start).count() << std::endl;
        E.saveMap("testMap.bin");
    }
}

#endif



