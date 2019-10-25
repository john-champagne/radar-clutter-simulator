#ifndef DEM_PARSER_H
#define DEM_PARSER_H

#include "elevation_reader.h"
#include "threevector.h"
#include "../options.h"

// Define EXPORTED for any platform
// This code copied from: https://atomheartother.github.io/c++/2018/07/12/CPPDynLib.html
#if defined _WIN32 || defined __CYGWIN__
  #ifdef WIN_EXPORT
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllexport))
    #else
      #define EXPORTED __declspec(dllexport) 
    #endif
  #else
    #ifdef __GNUC__
      #define EXPORTED __attribute__ ((dllimport))
    #else
      #define EXPORTED __declspec(dllimport)
    #endif
  #endif
  #define NOT_EXPORTED
#else
  #if __GNUC__ >= 4
    #define EXPORTED __attribute__ ((visibility ("default")))
    #define NOT_EXPORTED  __attribute__ ((visibility ("hidden")))
  #else
    #define EXPORTED
    #define NOT_EXPORTED
  #endif
#endif

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// chunk_t
// A single element of the terrain map.
typedef struct chunk_t {
    float r,az,el;         // Spherical Coordinates w.r.t. radar transmitter.
    uint8_t shadowed;
    float grazing;         // Grazing Angle.
} chunk_t;

// ElevationMap
// A class containing the map, with functions to calculate/populate the map.
class ElevationMap {
private:
    // Instance of the elevation reader.
    ElevationReader* ER;
    options_t* Options;
    // 2D arrays containing the map.
    chunk_t** map;
    float** elevation_map;

    // Coordinates of the origin in the map array
    int mapOriginX, mapOriginY;
    
    // The origin lat, lon, and height
    double originLat, originLon, originHeight;
	
    // The origin vector and x,y,z unit vectors.
    ThreeVector origin;
    ThreeVector axis[3];

    // The distance between chunks in meters.
    double deltaDistance = 30;
  
    // Calculates secondary parameters. 
    void calculateSecondaryParameters();

    // Calculates the longitude and latitude for a given array element.
    void calculateLatLon(int x, int y, float* lat, float* lon);
   
    // Calculates the spherical coordinates for a given lon,lat,height pair. 
    void calculateSphericalCoordinates(float lat, float lon,float h, float*az, float*el, float* r);
    
    void populateSphericalCoordinates(int start, int end);
    void populatePartial(int start, int end);    	
    // Grazing Angle Calculations
    double calculateDirectionalDerivative(float* h, float az);
    void calculateTerrainSlope();
    void populateGrazingAngle();
    void populateGrazingAnglePartial(int start, int end);
    
    // Shadowing Calculations
    void calculateShadowing();
    void calculateShadowingAlongLine(int x1, int y1, int x2, int y2);
     
    void allocateMap();
    void deallocateMap();
    void deallocateElevation();
    
    // Shared memory for multithreading.
    unsigned int threadCount; 
    int alloc_i;
    int elevation_i;
public:
    int mapSizeX, mapSizeY;
    void populateMap();

    // Accessor Functions
    chunk_t getMap(int x, int y);
    void setMap(int x, int y, chunk_t m);

    void exportMap();
    
    ElevationMap(options_t*);
    ~ElevationMap();

};


#endif
