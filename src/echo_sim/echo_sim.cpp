#include <assert.h>
#include <math.h>
#include <iostream>

using std::cout;
using std::endl;

#include <fstream>
#include <thread>
#include "echo_sim/echo_sim.h"
#include "echo_sim/clutter_coefficient.h"
#include "options.h"

/*EchoSimulator::EchoSimulator() {
    rangeBinCount     = ceil((1757.0)/(3.3333) + 13);
	azimuthCount      = 4096;
	rangeBinPeriod    = 3.333333333E-6;
	pulseInterval     = 13 * rangeBinPeriod;
	ERP               = dBmToWatt(86); 
}*/

EchoSimulator::EchoSimulator(options_t* O){
    Options = O;
    rangeBinCount   = Options->SIMULATOR_RANGE_BIN_COUNT;
    azimuthCount    = Options->SIMULATOR_AZIMUTH_ANGLE_COUNT;
    rangeBinPeriod  = Options->SIMULATOR_RANGE_BIN_PERIOD;
    ERP             = Options->SIMULATOR_TRANSMIT_POWER;
    pulseInterval   = Options->SIMULATOR_TRANSMIT_PULSE_LENGTH;
    map = new ElevationMap(O);
}

void EchoSimulator::PopulateAttenTable() {
    map->populateMap();
    AllocateAttenTable();
    if (Options->PROG_VERBOSE)
       cout << "Power Table Allocated." << endl; 
    int threadCount = std::thread::hardware_concurrency();
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
            if (chunk.r <= 100000 || chunk.shadowed == 1){ 
            float time1 = chunk.r*2.0/300000000.0;
            int RangeBinStart = time1/rangeBinPeriod;
            int RangeBinEnd = (time1 + pulseInterval)/rangeBinPeriod;
            // Isotropic Power = Radar equation without antenna gains.
            double IsotropicPower = ERP;
            IsotropicPower *= (30*30);
            IsotropicPower *= pow(300000000/200000000,2);
            IsotropicPower *= calculateClutterCoefficient(TerrainRural, chunk.grazing);
            IsotropicPower /= (pow(chunk.r,4)*pow(4*M_PI,3));
            assert(IsotropicPower >= 0.0);
            AddPowerReceived(   IsotropicPower * (1+RangeBinStart - time1/rangeBinPeriod),
                                chunk.az,
                                chunk.el,
                                RangeBinStart);
            AddPowerReceived(   IsotropicPower*(-1*RangeBinEnd + (time1+pulseInterval)/rangeBinPeriod),
                                chunk.az,
                                chunk.el,
                                RangeBinEnd);
            for (int k = RangeBinStart + 1; k < RangeBinEnd; k++)
                AddPowerReceived(IsotropicPower, chunk.az, chunk.el, k);
        }}
    }
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

float EchoSimulator::AntennaGain(float az, float el) {
    if (az >= -M_PI/2 && az <= M_PI/2)
        return pow(cos(az),2)*abs(cos(3*az));
    //return -1*cos(az)*abs(sin(3*az))/5.0;
    return 0;
}


void EchoSimulator::AddPowerReceived(double watts, float az, float el, int rangeBin) {
    // Lock the range bin mutex so that no other threads can write to it.
    #ifdef MEMORY_SAFE
    mutexTable[rangeBin].lock();
    #endif
    for (int i = 0; i < azimuthCount; i++) {
        // Apply the antenna pattern to each received echo.
        double p = watts * AntennaGain(GetRotatedAzimuthAngle(az,i), el);
        attenTable[rangeBin][i] += p;
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
    std::ofstream outputFile(filename, std::ios::out | std::ios::binary);
    outputFile.write((char*)&rangeBinCount, sizeof(rangeBinCount));
    outputFile.write((char*)&azimuthCount, sizeof(azimuthCount));

    for (int i = 0; i < rangeBinCount; i++)
        for (int j = 0; j < azimuthCount; j++) {
            double x = WattTodBm(attenTable[i][j]);
            outputFile.write((char*)&x,sizeof(x));
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
