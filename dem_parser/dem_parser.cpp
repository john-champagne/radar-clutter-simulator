#include "dem_parser.h"
#include <math.h>
#include <stdlib.h>

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
    ElevationReader ER;
    
    originLat = lat;
    originLon = lon;
    originHeight = ER.GetElevation(lat,lon) + height;
    
    mapSizeX = 2 * radius / deltaDistance;
    mapSizeY = 2 * radius / deltaDistance;
    allocateMap();
    mapOriginX = mapSizeX/2;
    mapOriginY = mapSizeY/2;

    for (int i = 0; i < mapSizeX; i++)
        for (int j = 0; j < mapSizeY; j++) {
            float lat2, lon2;
            calculateLatLon(i, j, &lat2, &lon2);
            map[i][j].elevation = ER.GetElevation(lat2, lon2);
        };

    origin = calculateECEF(originLat, originLon, originHeight);
   
    // x-axis reference
    axis[1] = calculateECEF(lat + 0.00001, lon, originHeight) - origin;
    axis[0] = calculateECEF(lat, lon - 0.00001, originHeight) - origin;
    
    // z-axis = x cross y 
    axis[2] = axis[1].cross(axis[0]);
    
    for (int i = 0; i < 3; i++)
        axis[i] = axis[i].normalize();

    calculateSecondaryParameters();
    calculateShadowing();
    calculateTerrainSlope();
    calculateGrazingAngle();
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
    float latTemp = originLat;
    float lonTemp = originLon;

    double walkAzimuth = 0;
    if (x != mapOriginX) {
        if (x < mapOriginX)
            walkAzimuth = 90;
        else if (x > mapOriginX)
            walkAzimuth = 270;
        ER.WalkDistance(    originLat, 
                            originLon, 
                            walkAzimuth, 
                            fabs(x-mapOriginX)*deltaDistance,
                            &latTemp, 
                            &lonTemp );
    }

    float latTemp2 = latTemp;
    float lonTemp2 = lonTemp;
    double walkAzimuth = 0;
    if (y != mapOriginY) {
        if (y < mapOriginY)
            walkAzimuth = 180;
        else if (y > mapOriginY)
            walkAzimuth = 0;
    }
    ER.WalkDistance (   latTemp, 
                        lonTemp, 
                        walkAzimuth,
                        fabs(y-mapOriginY)*deltaDistance,
                        lat, 
                        lon );
}


void ElevationMap::calculateSecondaryParameters() {
    double a = ElevationReader::a;
    double b = ElevationReader::b;
	
    for (int i = 0; i < mapSizeX; i++)
        for (int j = 0; j < mapSizeY; j++) {
            // Calculate ECEF coordinates.
            // Equations come from:
            // Wkipedia Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates
            map[i][j].ECEF = calculateECEF(map[i][j].lat, map[i][j].lon, map[i][j].elevation);
		    map[i][j].local.x = (map[i][j].ECEF - origin).dot(axis[0]);
            map[i][j].local.y = (map[i][j].ECEF - origin).dot(axis[1]);
            map[i][j].local.z = (map[i][j].ECEF - origin).dot(axis[2]);
    
			// Calculate radius, elevation angle, and azimuth angle relative to the transmitter.
			map[i][j].r  = sqrt(pow(map[i][j].local.x,2) + 
                                pow(map[i][j].local.y,2) + 
                                pow(map[i][j].local.z,2) );
			map[i][j].el = atan2((  map[i][j].local.z) , 
                                    sqrt(pow(map[i][j].local.x,2) + pow(map[i][j].local.y,2)));
			map[i][j].az = atan2((map[i][j].local.x), (map[i][j].local.y));
        }
}

/*  ElevationMap::allocateMap
    DESCRIPTION:
        Allocates the memory necessary for the map.
*/
void ElevationMap::allocateMap() {
    map = new chunk_t* [mapSizeX];
    for (int i = 0; i < mapSizeX; i++)
        map[i] = new chunk_t [mapSizeY];
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

chunk_t ElevationMap::getMap(int x, int y) {
    return map[x][y];
}

#ifdef DEBUG_DEM_PARSER

int main() {
    ElevationMap E;
    E.populateMap(38.52,-98.10,1000,0);
    for (int i = 0; i < E.mapSizeX; i++)
        for (int j = 0; j < E.mapSizeY; j++) {
            chunk_t m = E.getMap(i,j);
            printf("%f%c", m.r, (j == (E.mapSizeY - 1)) ? '\n' : ',');
        } 
}

#endif



