/* 
 * File:   srtm_elevation_reader.h
 * Author: Pavel Zbytovský <pavel@zby.cz>
 *
 * Created on April 28, 2013, 6:44 PM
 */

/* Modified by John Champagne */

#ifndef ELEVATIONREADER_H
#define	ELEVATIONREADER_H

#include <stdint.h>
#include <stdio.h>
#include "options.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


typedef struct _SrtmAscentDescent {
    float ascent;
    float descent;
    float ascentOn;
    float descentOn;
} TSrtmAscentDescent;

/* The elevation reader class.
 *
 * Description:
 * 	Reads elevation data, processes metadata such as coordinates,
 * 	shadowing, depression angle.
 * */
class ElevationReader {
    static constexpr int secondsPerPx = 3;  //arc seconds per pixel (3 equals cca 90m)
    static constexpr int totalPx = 1201;

    FILE* srtmFd;
    int srtmLat;
    int srtmLon;
    unsigned char * srtmTile;
    
private:
    options_t* Options;
    char folder[1024];

    void LoadTileInMemory (int latDec, int lonDec);
    void ReadHeightFromTile (int y, int x, int* height);

    void srtmClose();
public:
    // Major and Minor axis:
    static constexpr double a = 6378137.0;
    static constexpr double b = 6356752.314245;

    float GetElevation(float lat, float lon);


    TSrtmAscentDescent GetAscentDescent(float lat1, float lon1, float lat2, float lon2, float dist);

    double GetDistance(double lat1, double lon1, double lat2, double lon2);
    void WalkDistance(double lat, double lon, double az, double s, double*lat_out, double* lon_out);

    ElevationReader();
    ElevationReader(options_t*);

    ~ElevationReader();
};


#endif
