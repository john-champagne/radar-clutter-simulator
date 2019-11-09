#include <assert.h>
#include <math.h>
#include <iostream>

using std::cout;
using std::endl;

#include <fstream>
#include <thread>
#include "echo_sim/echo_sim.h"
#include "echo_sim/clutter_coefficient.h"
#include "echo_sim/antenna_pattern.h"
#include "options.h"


EchoSimulator::EchoSimulator(options_t* O){
    Options = O;
    rangeBinCount   = Options->SIMULATOR_RANGE_BIN_COUNT;
    azimuthCount    = Options->SIMULATOR_AZIMUTH_ANGLE_COUNT;
    rangeBinPeriod  = Options->SIMULATOR_RANGE_BIN_PERIOD;
    ERP             = Options->SIMULATOR_TRANSMIT_POWER;
    pulseInterval   = Options->SIMULATOR_TRANSMIT_PULSE_LENGTH;
    map = new ElevationMap(O);
    if (Options->SIMULATOR_ANTENNA_FILENAME == "")
        pattern = new AntennaPatternAnalytical();
    else
        pattern = new AntennaPatternFile(Options->SIMULATOR_ANTENNA_FILENAME);
}

void EchoSimulator::PopulateAttenTable() {
    map->populateMap();
    AllocateAttenTable();
    if (Options->PROG_VERBOSE)
       cout << "Power table allocated." << endl; 
    int threadCount = Options->SIMULATOR_THREAD_COUNT;
    std::thread threads[threadCount];
    float mapDelta = float(map->mapSizeX-1)/float(threadCount);
    threads[0] = std::thread(&EchoSimulator::PopulateAttenTablePartial, this, 0, int(mapDelta));
    for (int i = 1; i < threadCount; i++) 
        threads[i] = std::thread(   &EchoSimulator::PopulateAttenTablePartial,this, 
                                    int(mapDelta*i)+1,
                                    int(mapDelta*(i+1))
                                    );
    for (int i = 0; i < threadCount; i++)
        threads[i].join();
}


void EchoSimulator::PopulateAttenTablePartial(int start, int end) {
    for (int j = start; j <= end; j++) {
        for (int i = 0; i < map->mapSizeX; i++) {
            
            chunk_t chunk = map->getMap(i,j);
            if (chunk.shadowed == 0) { 
                float time1 = chunk.r*2.0/Options->SIMULATOR_WAVE_SPEED;
                int RangeBinStart = time1/rangeBinPeriod;
                int RangeBinEnd = (time1 + pulseInterval)/rangeBinPeriod;
                // Isotropic Power = Radar equation without antenna gains.
                double IsotropicPower = ERP;
                
                // Wavelength in meters.
                double wavelength = Options->SIMULATOR_WAVE_SPEED/Options->SIMULATOR_TRANSMIT_FREQUENCY;
                IsotropicPower *= pow(wavelength,2);
                
                // Radar Cross Section = A * sigma0
                IsotropicPower *= (30*30);
                IsotropicPower *= calculateClutterCoefficient(TerrainRural, chunk.grazing);
                
                // 1/R^4, 1/(4pi)^3
                IsotropicPower /= (pow(chunk.r,4)*pow(4*M_PI,3));
                assert(IsotropicPower >= 0.0);
                AddPowerReceived(   IsotropicPower * (1+RangeBinStart - time1/rangeBinPeriod),
                                    chunk.az,
                                    chunk.el,
                                    RangeBinStart,RangeBinStart);
                AddPowerReceived(   IsotropicPower*(-1*RangeBinEnd + (time1+pulseInterval)/rangeBinPeriod),
                                    chunk.az,
                                    chunk.el,
                                    RangeBinEnd,RangeBinEnd);
                AddPowerReceived(IsotropicPower, chunk.az, chunk.el, RangeBinStart + 1, RangeBinEnd-1);
            }
        }
    }
    if (Options->PROG_VERBOSE)
        cout << ".";
}

void EchoSimulator::AllocateAttenTable() {
    attenTable = new double* [rangeBinCount];
    mutexTable = new std::mutex [rangeBinCount];
    for (int i = 0; i < rangeBinCount; i++)
        attenTable[i] = new double [azimuthCount];
}

float EchoSimulator::GetRotatedAzimuthAngle(float az, int azBin) {
    float new_az = az - azBin*2*M_PI/azimuthCount;
    while (new_az < -M_PI)
        new_az += 2*M_PI;
    while (new_az > M_PI)
        new_az -= 2*M_PI;
    return new_az;
}

void EchoSimulator::AddPowerReceived(double watts, float az, float el, int rangeBinStart, int rangeBinEnd) {
    int i_min = (int)(((az - pattern->angleMax())/(2*M_PI)*azimuthCount + azimuthCount))%azimuthCount;
    int i_max = (int)(((az - pattern->angleMin())/(2*M_PI)*azimuthCount + azimuthCount))%azimuthCount;
    // Lock the range bin mutex so that no other threads can write to it.
    #ifdef MEMORY_SAFE
    mutexTable[rangeBinStart].lock();
    #endif
    for (int i = i_min; i != i_max; i = (i+1)%azimuthCount) {
        // Apply the antenna pattern to each received echo.
        double g = pattern->Gain(GetRotatedAzimuthAngle(az,i), el);
        if (g != 0) {
            double p = watts * g;
            for (int j = rangeBinStart; j <= rangeBinEnd; j++)
                attenTable[j][i] += p;
        }
    }
    #ifdef MEMORY_SAFE
    mutexTable[rangeBin].unlock();
    #endif
}

void EchoSimulator::SaveCSV(const char* filename){
    for (int i = 0; i < rangeBinCount; i++) 
        for (int j = 0; j < azimuthCount; j++){
            
            std::cout << WattTodBm(attenTable[i][j]);
            if (j == (azimuthCount - 1))
                std::cout << "\n";
            else
                std::cout << ", ";
        }
}

void EchoSimulator::SaveToFile(const char* filename) {
    std::ofstream outputFile(Options->SIMULATOR_OUTPUT_FILENAME.c_str(), std::ios::out | std::ios::binary);
    //outputFile.write((char*)&rangeBinCount, sizeof(rangeBinCount));
    //outputFile.write((char*)&azimuthCount, sizeof(azimuthCount));

    for (int i = 0; i < rangeBinCount; i++)
        for (int j = 0; j < azimuthCount; j++) {
            outputFile << WattTodBm(attenTable[i][j]);
            if (j == azimuthCount - 1)
                outputFile << "\n";
            else
                outputFile << ", ";
        }
    outputFile.close();
}

#ifdef DEBUG_ECHO_SIM

#include <stdio.h>

int main() {
    EchoSimulator ES;
    ES.PopulateAttenTable();
    ES.SaveCSV("out.csv");
    ES.SaveToFile("out.bin");
}

#endif
