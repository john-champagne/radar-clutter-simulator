/* 
 * File:   srtmHgtReader.cpp
 * Author: Pavel Zbytovský <pavel@zby.cz>
 *
 * Created on April 28, 2013, 12:01 AM
 */

#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>

#include <fstream>
#include <string>
#include <streambuf>
#include <iostream>
#include <assert.h>

#include <math.h>
#include <string.h>

#include "dem_parser/elevation_reader.h"

#define D_TO_R M_PI / 180.0f
#define R_TO_D 180.0f / M_PI

ElevationReader::ElevationReader(){
    srtmFd = NULL;
    srtmLat = 255; //default never valid
    srtmLon = 255;
    srtmTile = NULL;

}

ElevationReader::ElevationReader(options_t* O){
    Options = O;
    srtmFd = NULL;
    srtmLat = 255; //default never valid
    srtmLon = 255;
    srtmTile = NULL;
    strcpy(folder, Options->DEM_PARSER_SRTM_FOLDER.c_str());
}

ElevationReader::~ElevationReader(){
    srtmClose();
}

/** Prepares corresponding file if not opened */
void ElevationReader::LoadTileInMemory(int latDec, int lonDec){
    if(srtmLat != latDec || srtmLon != lonDec) {
        srtmLat = latDec;
        srtmLon = lonDec;
        
        if(srtmFd != NULL){
            fclose(srtmFd);
        }
        
        char filename[1024];
        sprintf(filename, "%s/%c%02d%c%03d.hgt", folder, 
					latDec>0?'N':'S', abs(latDec), 
					lonDec>0?'E':'W', abs(lonDec));
					
        srtmFd = fopen(filename, "r");
        
        if(srtmFd == NULL) {
            printf("Error opening %s\n",  filename);
            exit(1);
        }
        
#if !SRTMSLIM
        if(srtmTile == NULL){
            srtmTile = (unsigned char*) malloc(totalPx * totalPx * 2); //allocate only once

        }
        
        //read the whole tile
        fread(srtmTile, 1, (2 * totalPx * totalPx), srtmFd);
#endif
    }
}

void ElevationReader::srtmClose() {
    if(srtmFd != NULL){
        fclose(srtmFd);
    }
    
#if !SRTMSLIM
    if(srtmTile != NULL){
        free(srtmTile);
    }
#endif
}

/** Pixel idx from left bottom corner (0-1200) */
void ElevationReader::ReadHeightFromTile(int y, int x, int* height){
    int row = (totalPx-1) - y;
    int col = x;
    int pos = (row * totalPx + col) * 2;
    
#if SRTMSLIM
    
    //seek and read 2 bytes short
    unsigned char buff[2];// = {0xFF, 0xFB}; //-5 (bigendian)
    fseek(srtmFd, pos, SEEK_SET);
    fread(&buff, 2, 1, srtmFd);
    
#else
    
    //set correct buff pointer
    unsigned char * buff = & srtmTile[pos];
    
#endif
    
    //solve endianity (using int16_t)
    int16_t hgt = 0 | (buff[0] << 8) | (buff[1] << 0);
    
    if(hgt == -32768) {
        hgt = prevHeight;
    }
    prevHeight = (int) hgt;
    *height = (int) hgt;
}       

/** Returns interpolated height from four nearest points */

/** ElevationReader::GetElevation
 * DESCRIPTION:
 *      Reads the elevation at a specific point on the earth.
 * ARGUMENTS:
 *      double lat, lon
 *          The lattitude and longitude of the point in degrees.
 * RETURNS:
 *      a float, representing the elevation.
 */
float ElevationReader::GetElevation(float lat, float lon){
    if (!Options->DEM_PARSER_DISABLE_ELEVATION) {
        int latDec = (int)floor(lat);
        int lonDec = (int)floor(lon);

        float secondsLat = (lat-latDec) * 60 * 60;
        float secondsLon = (lon-lonDec) * 60 * 60;
        
        LoadTileInMemory(latDec, lonDec);
        //X coresponds to x/y values,
        //everything easter/norhter (< S) is rounded to X.
        //
        //  y   ^
        //  3   |       |   S
        //      +-------+-------
        //  0   |   X   |
        //      +-------+-------->
        // (sec)    0        3   x  (lon)
        
        //both values are 0-1199 (1200 reserved for interpolating)
        int y = secondsLat/secondsPerPx;
        int x = secondsLon/secondsPerPx;
        
        //get norther and easter points
        int height[4];
        ReadHeightFromTile(y,   x, &height[2]);
        ReadHeightFromTile(y+1, x, &height[0]);
        ReadHeightFromTile(y,   x+1, &height[3]);
        ReadHeightFromTile(y+1, x+1, &height[1]);

        //ratio where X lays
        float dy = fmod(secondsLat, secondsPerPx) / secondsPerPx;
        float dx = fmod(secondsLon, secondsPerPx) / secondsPerPx;
        
        // Bilinear interpolation
        // h0------------h1
        // |
        // |--dx-- .
        // |       |
        // |      dy
        // |       |
        // h2------------h3   
        return  height[0] * dy * (1 - dx) +
                height[1] * dy * (dx) +
                height[2] * (1 - dy) * (1 - dx) +
                height[3] * (1 - dy) * dx;
    } else {
        double tx = 3.141592/180.0;
        double d = GetDistance(lat*tx, lon*tx, 38.52*tx, -98.10*tx);
        d = d/100;
        return 1; // Debug mode for if database is not accesssible
    }
}

/** ElevationReader::GetDistance
 * DESCRIPTION:
 *      Gets the distance along the Earth's surface between two points
 *      defined by longitude/latitude.
 *
 *      This is performed using Vincenty's Formula (specifically, the inverse problem)
 *      See: https://en.wikipedia.org/wiki/Vincenty%27s_formulae#Inverse_problem
 * ARGUMENTS:
 *      double lat1, lon1
 *          The lattitude and longitude of the first point in degrees.
 *      double lat2, lon2
 *          The lattidude and longitude of the second point in degrees.
 * RETURNS:
 *      a double, representing the distance between the two points in meters.
 */
double ElevationReader::GetDistance(double lat1, double lon1, double lat2, double lon2) {
    double f = (a-b)/a;
    double tan_U1 = (1-f) * tan(lat1);
    double tan_U2 = (1-f) * tan(lat2);
    double S;
    // Longitudinal Distance
    double cos_U1 = 1 / sqrt(1 + tan_U1 * tan_U1);
    double cos_U2 = 1 / sqrt(1 + tan_U2 * tan_U2);
    double sin_U1 = tan_U1 * cos_U1;
    double sin_U2 = tan_U2 * cos_U2;

    // Iterate until complete
    double L = lon2 - lon1;
    double lambda = L;
    double diff, sigma;
    double cos_alpha_sq, cos_2sigma_m;
    double cos_sigma, sin_sigma;

    while( true ){
        double sin_lambda = sin( lambda );
        double cos_lambda = cos( lambda );

        double c1 = (cos_U2 * sin_lambda)*(cos_U2 * sin_lambda);
        double c2 = (cos_U1 * sin_U2);
        double c3 = (sin_U1 * cos_U2 * cos_lambda);

        sin_sigma = sqrt( c1 + ( c2 - c3 )*( c2 - c3 ) );
        cos_sigma = sin_U1 * sin_U2 + cos_U1 * cos_U2 * cos_lambda;
        sigma = atan2( sin_sigma, cos_sigma );
        double sin_alpha = (cos_U1 * cos_U2 * sin_lambda)/(sin_sigma);
        cos_alpha_sq = 1 - (sin_alpha*sin_alpha);
        cos_2sigma_m = cos_sigma - (2 * sin_U1 * sin_U2)/(cos_alpha_sq);
        double C = (f/16.0) * cos_alpha_sq * (4 + f * (4 - 3 * cos_alpha_sq));
        diff = lambda;
        
        lambda = L + (1-C) * f * sin_alpha * (sigma + C * sin_sigma * 
                 ( cos_2sigma_m + C * cos_sigma * (-1 + 2 * cos_2sigma_m*cos_2sigma_m)));
        
        diff = lambda - diff;
        if( fabs(diff) < 0.00001 ){ break; }
    }
    double u_sq = cos_alpha_sq  * (a*a - b*b)/(b*b);
    double A = 1 + (u_sq/16384) * (4096 + u_sq * (-768 + u_sq * (320 - 175 * u_sq)));

    double B = (u_sq / 1024) * (256 + u_sq * (-128 + u_sq * (-128 + u_sq * (74 - 47 * u_sq))));
    double cos_2sigma_m_sq = cos_2sigma_m * cos_2sigma_m;
    
    double delta_sigma = B * sin_sigma * ( cos_2sigma_m + (B/4.0) * 
                        (cos_sigma * (-1 * 2 * cos_2sigma_m_sq ) - (B/6.0) * 
                         cos_2sigma_m * (-3 + 4 * sin_sigma*sin_sigma) * 
                         (-3 + 4 * cos_2sigma_m_sq)));
    return (b * A * (sigma - delta_sigma)); 
}

void ElevationReader::WalkDistance(double lat, double lon, double az, double s, double* lat_out, double* lon_out) {
   	lat = lat* D_TO_R;
	lon = lon* D_TO_R;
	az =  az*  D_TO_R;
	double eu2 = (pow(a, 2) - pow(b, 2)) / pow(b, 2);
	double v = sqrt(1 + eu2);
	double beta1 = atan(tan(lat)/v);
	double Aeq = asin(cos(beta1) * sin(az));
	double f = (a - b) / a;
	double u2 = pow(cos(Aeq),2)*eu2;
	//----------------------------------------------
	double sigma1 = atan2( tan(beta1), cos(az) );
	double A = 1 + u2/16384*(4096 + u2*(-768+u2*(320-175*u2)));
	double B = u2/1024*(256 + u2*(-128+u2*(74-47*u2)));
	double SIGMA[2];
	SIGMA[0] = s /  (b*A);
	SIGMA[1] = 1e20;
	double sigmaM;
	double d_w;
	double d_sigma;
	//----------------------------------------------
	double C;
	double d_lon;
	static int iteration = 0;
	//-----------------ITERATION------------------//
	while( true )
    {
        sigmaM = 2*sigma1 + SIGMA[0]; //ACTUALLY TWO SIGMA-M
        d_sigma = B*sin(SIGMA[0])*(cos(sigmaM)+B/4*(cos(SIGMA[0])*
                    (-1+2*pow(cos(2*SIGMA[0]),2)) -B/6*cos(sigmaM)*
                    (-3+4*pow(sin(SIGMA[0]),2))*(-3+4*pow(cos(sigmaM),2))));

        SIGMA[1] = SIGMA[0];
        SIGMA[0]  = s /  (b*A) + d_sigma;
        iteration++;

        if( fabs(SIGMA[1] - SIGMA[0]) < 1e-10 )
            break;
    }
	 //-------------END OF ITERATION--------------//
	//*iter = iteration;
	*lat_out = atan2((sin(beta1)*cos(SIGMA[1])+cos(beta1)*sin(SIGMA[1])*cos(az)),((1-f)
	*sqrt(pow(sin(Aeq),2)+pow((sin(beta1)*sin(SIGMA[1])-cos(beta1)*cos(SIGMA[1])*cos(az)),2)))) * R_TO_D/**180/M_PI*/;

	d_w = atan2((sin(SIGMA[1])*sin(az)),(cos(beta1)*cos(SIGMA[1]) - sin(beta1)* sin(SIGMA[1])*cos(az)));
	C 	= f/16*pow(cos(Aeq),2)*(4+f*(4-3*pow(cos(Aeq),2)));
	d_lon = d_w - (1-C)*f*sin(az)*(SIGMA[1] + C*sin(SIGMA[1])*(cos(sigmaM)+C*cos(SIGMA[1])*(-1+2*pow(cos(sigmaM),2))));
	*lon_out = (lon + d_lon) * R_TO_D;
}