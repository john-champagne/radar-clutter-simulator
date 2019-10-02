#include "dem_parser.h"
#include <math.h>
#include <stdlib.h>

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
    
    mapSizeX = 2 * radius / deltaDistance;
    mapSizeY = 2 * radius / deltaDistance;
    allocateMap();
    mapOriginX = mapSizeX/2;
    mapOriginY = mapSizeY/2;
    
    origin = calculateECEF(originLat, originLon, originHeight);
   
    // x-axis reference
    axis[1] = calculateECEF(lat + 0.00001, lon, originHeight) - origin;
    axis[0] = calculateECEF(lat, lon - 0.00001, originHeight) - origin;
    
    // z-axis = x cross y 
    axis[2] = axis[1].cross(axis[0]);
    
    for (int i = 0; i < 3; i++)
        axis[i] = axis[i].normalize();
    
    for (int i = 0; i < mapSizeX; i++)
        for (int j = 0; j < mapSizeY; j++) {
            float lat2, lon2;
            calculateLatLon(i, j, &lat2, &lon2);
            elevation_map[i][j] = ER.GetElevation(lat2, lon2);
        };
   
    // Populate the azimuth, elevation, and range of the map using multithreading. 
    unsigned int threadCount = std::thread::hardware_concurrency();
    std::thread threads[threadCount];
    float mapDelta = float(mapSizeX-1)/float(threadCount);
    threads[0] = std::thread(&ElevationMap::populateSphericalCoordinates,this, 0, int(mapDelta));
    for (int i = 1; i < threadCount; i++)
        threads[i] = std::thread(   &ElevationMap::populateSphericalCoordinates,this, 
                                    int(mapDelta*i)+1,
                                    int(mapDelta*(i+1))
                                );
    // Wait for all threads to finish running.
    for (int i = 0; i < threadCount; i++)
        threads[i].join();
        
    //calculateShadowing();
    //calculateGrazingAngle();
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
 *      int x, y
 *          The element of the map. 
 *      float* az, el, r
 *          Pointers to the azimuth angle, elevation angle, and range. 
 */
void ElevationMap::calculateSphericalCoordinates(int i, int j, float* az, float* el, float* r) {
    float lat, lon;
    calculateLatLon(i, j, &lat, &lon);
    ThreeVector local = calculateECEF(lat, lon, elevation_map[i][j]);
    
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
    for (int i = start; i <= end; i++)
        for (int j = 0; j < mapSizeY; j++)
            calculateSphericalCoordinates(i,j, &map[i][j].az, &map[i][j].el, &map[i][j].r);
}

/*  ElevationMap::allocateMap
    DESCRIPTION:
        Allocates the memory necessary for the map.
*/
void ElevationMap::allocateMap() {
    map = new chunk_t* [mapSizeX];
    for (int i = 0; i < mapSizeX; i++)
        map[i] = new chunk_t [mapSizeY];

    elevation_map = new float* [mapSizeX];
    for (int i = 0; i < mapSizeY; i++)
        elevation_map[i] = new float [mapSizeY];
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

#ifdef DEBUG_DEM_PARSER

int main() {
    for (int i = 1; i < 2; i++) {
        auto start = std::chrono::steady_clock::now();
        ElevationMap E;
        E.populateMap(38.52, -98.10, 264000,0);
        auto end = std::chrono::steady_clock::now();
        std::cout  << (end - start).count() << std::endl;
    }
}

#endif



