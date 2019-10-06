#include <math.h>
#include <thread>
#include "echo_sim.h"
#include "clutter_coefficient.h"


EchoSimulator::EchoSimulator() {
    rangeBinCount     = ceil((1757.0)/(3.3333) + 75);
	azimuthCount      = 4096;
	rangeBinPeriod    = 3.333333333E-6;
	pulseInterval     = 250e-6;
	ERP               = dBmToWatts(86); 
}

void EchoSimulator::PopulateAttenTable() {
    map.populateMap(38.52, -98.10, 10000,10);
    AllocateAttenTable();

    threadCount = std::thread::hardware_concurrency();
    std::thread threads[threadCount];
    float mapDelta = float(map.mapSizeX-1)/float(threadCount);
    threads[0] = std::thread(&ElevationMap::populatePartial, this, 0, int(mapDelta));
    for (int i = 1; i < threadCount; i++) 
        threads[i] = std::thread(   &EchoSimulator::PopulateAttenTablePartial,this, 
                                    int(mapDelta*i)+1,
                                    int(mapDelta*(i+1))
                                    );
    for (int i = 0; i < threadCount; i++)
        threads[i].join();
}


void EchoSimulator::PopulateAttenTablePartial(int start, int end) {
    for (int j = start; j <= end; j++)
        for (int i = 0; i < mapSizeX; i++) {
            chunk_t chunk = map.getMap(i,j);
            float time1 = chunk.r*2.0/300000000.0;
            int RangeBinStart = time1/rangeBinPeriod;
            int RangeBinEnd = (time1 + pulseInterval)/rangeBinPeriod;
            double IsotropicPower =     ERP*30*30*pow(2000000.0/300000000.0,2)
                                        calculateClutterCoefficient(TerrainRural, chunk.grazing)/(pow(chunk.r,4)*pow(4*M_PI,3));
            AddPowerReceived(   IsotropicPower * (1+RangeBinStart - time1/rangeBinPeriod),
                                chunk.az,
                                chunk.el,
                                RangeBinStart);
            AddPowerReceived(   IsotropicPower*(RangeBinEnd - (time+pulseInterval)/rangeBinPeriod),
                                chunk.az,
                                chunk.el,
                                RangeBinEnd);
            for (int i = RangeBinStart + 1; i < RangeBinEnd; i++)
                AddPowerReceived(IsotropicPower, chunk.az, chunk.el, i);
        }
}

void EchoSimulator::AllocateAttenTable() {
    attenTable = new double* [rangeBinCount];
    mutexTable = new std::mutex [rangeBinCount];
    for (int i = 0; i < rangeBinCount; i++)
        attenTable[i] = new double [azimuthCount];
}

void EchoSimulator::GetRotatedAzimuthAngle(float az, int azBin) {
    float new_az = az - azBin*2*M_PI/azimuthCount;
    while (new_az < 0)
        new_az += 2*M_PI;
    return new_az;
}

void AntennaGain(float az, float el) {
    if (az >= -M_PI/2 && az <= M_PI/2)
        return pow(cos(az),3)*abs(cos(3*az));
    return -1*cos(az)*abs(sin(3*az))/5.0;
}


void EchoSimulator::AddPowerReceived(double watts, float az, float el, int rangeBin) {
    // Lock the range bin mutex so that no other threads can write to it.
    mutexTable[rangeBin].lock();
    for (int i = 0; i < azimuthCount; i++) {
        // Apply the antenna pattern to each received echo.
        attenTable[rangeBin][i] += watts * AntennaGain(GetRotatedAzimuthAngle(az,i), el);
    }
    mutexTable[rangeBin].unlock();
}
