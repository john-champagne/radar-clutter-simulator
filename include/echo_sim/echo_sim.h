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
#include <mutex>
#include "../dem_parser/elevation_reader.h"
#include "../dem_parser/dem_parser.h"
#include "echo_sim/antenna_pattern.h"

class EchoSimulator {
private:
    ElevationMap* map;
    AntennaPattern* pattern;
    options_t* Options;
        
    uint16_t rangeBinCount;
    uint16_t azimuthCount;
    float rangeBinPeriod;
    float pulseInterval;
    float ERP;
        
    double** attenTable;
    std::mutex* mutexTable;

    void AddPowerReceived(double , float, float, int);
    float GetRotatedAzimuthAngle(float az, int azBin); 

public:
    EchoSimulator(options_t*);
   
    void PopulateAttenTable();
    void PopulateAttenTablePartial(int start, int end);

    void SaveCSV(const char* filename);
    void SaveToFile(const char* filename); 
	void LoadElevationPattern(const char* filename);
    void AllocateAttenTable();	
};

#endif
