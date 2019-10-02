#ifdef DEBUG_TERRAIN_SLOPE 
#include <iostream>
#include <assert.h>
#include <string>
using namespace std;
#endif

#include "dem_parser.h"
#include <math.h>


// This is the inverse matrix described in the final design.
// It is used to calculate the polynomial coefficients.
// This is precalculated using a D value of 30m.
double inverseHeightMatrix[9][9] = {
	{0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 , 0.0 , 0.0 , 0.0 },
	{0.0 , 0.0 , 0.0 , -0.016666666666666666 , 0.0 , 0.016666666666666666 , 0.0 , 0.0 , 0.0 },
	{-0.0 , -0.0 , -0.0 , 0.0005555555555555556 , -0.0011111111111111111 , 0.0005555555555555556 , -0.0 , -0.0 , -0.0 },
	{0.0 , -0.016666666666666666 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.016666666666666666 , 0.0 },
	{0.0002777777777777778 , 0.0 , -0.0002777777777777778 , 0.0 , 0.0 , 0.0 , -0.0002777777777777778 , 0.0 , 0.0002777777777777778 },
	{-9.259259259259259e-06 , 1.8518518518518518e-05 , -9.259259259259259e-06 , -0.0 , -0.0 , -0.0 , 9.259259259259259e-06 , -1.8518518518518518e-05 , 9.259259259259259e-06 },
	{-0.0 , 0.0005555555555555556 , -0.0 , -0.0 , -0.0011111111111111111 , -0.0 , -0.0 , 0.0005555555555555556 , -0.0 },
	{-9.259259259259259e-06 , -0.0 , 9.259259259259259e-06 , 1.8518518518518518e-05 , -0.0 , -1.8518518518518518e-05 , -9.259259259259259e-06 , -0.0 , 9.259259259259259e-06 },
	{3.08641975308642e-07 , -6.17283950617284e-07 , 3.08641975308642e-07 , -6.17283950617284e-07 , 1.234567901234568e-06 , -6.17283950617284e-07 , 3.08641975308642e-07 , -6.17283950617284e-07 , 3.08641975308642e-07 }
};

/** ElevationMap::calculateDirectionalDerivative
 * DESCRIPTION:
 *      Calculates a directional derivative used for the terrain slope angle.
 * ARGUMENTS:
 *      double* h
 *          An array which holds the 9 elevation levels described in the design documentation.
 *      double az
 *          The azimuth angle in radians.
 * RETURNS:
 *      double
 *          The directional derivative.
 */
double ElevationMap::calculateDirectionalDerivative(float* h, float az) {
	double b[9];
	for (int i = 0; i < 9; i++) {
		b[i] = 0;
		for (int j = 0; j < 9; j++)
			b[i] += inverseHeightMatrix[i][j] * h[j];
	}
	
	#ifdef DEBUG_TERRAIN_SLOPE 
    // if you understand this, you're a nerd.
	cout << "B vector:" << endl;
	for (int i = 0; i < 9; i++) {
		cout << b[i] << endl;
	}
	cout << endl;
	cout << endl;
	for (int i = 0; i < 9; i++) {
		const double D = 30;
		double h_predicted = 0;
		double x = ((i%3)-1)*D;
		double y = ((i/3)-1)*D;
		for (int j = 0; j < 9; j++) {
			h_predicted += b[j] * pow(x,(j%3)) * pow(y, (j/3));
		}
		cout << i << ": \t"<< "[" << x << "," << y << "]\t" << "h_pred = " << h_predicted << "   h = " << h[i] << endl;
		assert(abs(h_predicted - h[i]) < 0.000001);
	}
	#endif

	return sin(az)*b[1] + cos(az)*b[3];
}

void ElevationMap::calculateGrazingAngle() {
    // Calculate the elevation slope on the map.
    for (int i = 1; i < mapSizeX - 1; i++) {
        for (int j = 1; j < mapSizeX - 1; j++) {
            float h[9];
            for (int k = 0; k < 9; k++) 
                h[k] = elevation_map[i + (k%3 - 1)][j + (k/3 - 1)];
            map[i][j].grazing = atan(   calculateDirectionalDerivative((float*)h, 
                                        map[i][j].az)) - map[i][j].el;
        }
    }
    // Calculate the terrain slope on the edges using a simple moving average.
    for (int i = 1; i < mapSizeX - 1; i++){
        map[i][0].grazing = 1.0/3.0 * ( map[i][1].grazing + 
                                        map[i-1][1].grazing + 
                                        map[i + 1][1].grazing );
        map[i][mapSizeY-1].grazing = 1.0/3.0 * (    map[i][mapSizeY-2].grazing + 
                                                    map[i-1][mapSizeY-2].grazing + 
                                                    map[i + 1][mapSizeY-2].grazing );
        map[0][i].grazing = 1.0/3.0 * ( map[1][i].grazing + 
                                        map[1][i-1].grazing + 
                                        map[1][i+1].grazing );
        map[mapSizeX-1][i].grazing = 1.0/3.0 * (    map[mapSizeX-2][i].grazing + 
                                                    map[mapSizeX-2][i+1].grazing + 
                                                    map[mapSizeX-2][i-1].grazing );
        

    }
    // Calculate the terrain slope of the corners using SMA.
    map[0][0].grazing = 1.0/3.0 * ( map[1][0].grazing + 
                                    map[1][1].grazing + 
                                    map[0][1].grazing
                                  );

    map[mapSizeX-1][0].grazing = 1.0/3.0 * (    map[mapSizeX-2][0].grazing + 
                                                map[mapSizeX-2][1].grazing + 
                                                map[mapSizeX-1][1].grazing
                                            );

    map[0][mapSizeY-1].grazing = 1.0/3.0 * (    map[0][mapSizeY-2].grazing + 
                                                map[1][mapSizeY-2].grazing + 
                                                map[1][mapSizeY-1].grazing
                                            );
    map[mapSizeX-1][mapSizeY-1].grazing = 1.0/3.0 * (   map[mapSizeX-1][mapSizeY-2].grazing + 
                                                        map[mapSizeX-2][mapSizeY-2].grazing + 
                                                        map[mapSizeX-2][mapSizeY-1].grazing
                                            );
}

#ifdef DEBUG_TERRAIN_SLOPE 

int main() {
	for (int i = 0; i < 100; i++) {
		double h[9] = {0,0,0,0,0,0,0,0,0};
		for (int j = 0; j < 9; j++)
			h[j] = (rand() % 100) - 50;
		calculateDirectionalDerivative(h,0);
	}
}

#endif
