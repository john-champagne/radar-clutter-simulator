/*
Copyright © 2019 John Champagne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 * File Name:       echo_sim.h
 * File Author:     John Champagne
 * Date Created:    Oct 2 2019
 */

#ifndef ECHO_SIM_H
#define ECHO_SIM_H
#include <math.h>
#include "../dem_parser/elevation_reader.h"

typedef struct AntennaPattern_t {
	int az_size;
	int el_size;
	float* el, az;
} AntennaPattern_t;

class EchoSimulator {
private:
	ElevationMap map;
	AntennaPattern_t pattern;
	
	int rangeBinCount;
	int azimuthCount;
	float rangeBinPeriod;
	float pulseInterval;
	float ERP;
	
	float** attenTable;
	
public:
	EchoSimulator();
	
	void LoadElevationPattern(const char* filename);
	
}

#endif