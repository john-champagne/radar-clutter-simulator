#ifndef DEM_PARSER_H
#define DEM_PARSER_H

#include "elevation_reader.h"
#include "threevector.h"

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


typedef struct chunk_t {
    float r,az,el;         // Spherical Coordinates w.r.t. radar transmitter.
    uint8_t shadowed;
    float grazing;         // Grazing Angle.
} chunk_t;


class ElevationMap {
private:
    ElevationReader ER;
    
    chunk_t** map;
    float** elevation_map;
    
    int mapOriginX, mapOriginY;
    

    double originLat, originLon, originHeight;
	ThreeVector origin;
    ThreeVector axis[3];

    static constexpr double deltaDistance = 30;
   
    void calculateSecondaryParameters();

    void calculateLatLon(int x, int y, float* lat, float* lon);
    void calculateSphericalCoordinates(int x, int y, float* az, float* el, float* r);
    void populateSphericalCoordinates(int start, int end);
    	
    // Grazing Angle Calculations
    double calculateDirectionalDerivative(float* h, float az);
    void calculateTerrainSlope();
    void calculateGrazingAngle();
    
    // Shadowing Calculations
    void calculateShadowing();
    void calculateShadowingAlongLine(int x1, int y1, int x2, int y2);
     
    void allocateMap();
    void deallocateMap();
    void deallocateElevation();
public:
    int mapSizeX, mapSizeY;
    void populateMap(double lat, double lon, double radius, double height);

    // Accessor Functions
    chunk_t getMap(int x, int y);
    void setMap(int x, int y, chunk_t m);

};


#endif
