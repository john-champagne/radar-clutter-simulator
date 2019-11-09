#include <fstream>
#include <math.h>
#include <stdio.h>
#include "echo_sim/antenna_pattern.h"


float AntennaPatternAnalytical::angleMax() {
    return M_PI/4.0;
}

float AntennaPatternAnalytical::angleMin() {
    return -1*M_PI/4.0;
}

float AntennaPatternAnalytical::Gain(float azimuth, float elevation){
    if (azimuth >= -M_PI/2 && azimuth <= M_PI/2)
        return pow(cos(azimuth),2)*abs(cos(3*azimuth));
    return 0;
}

/*
    0       File version.
    1-4     Length of antenna pattern.
*/
AntennaPatternFile::AntennaPatternFile(std::string filename) {
    using std::ifstream;
    using std::ios;
    using std::string;
    ifstream input_file(filename, ios::in | ios::binary);
    if (input_file.is_open()) {
        uint8_t file_version = 0;
        sample_number = 0;
        input_file.read((char*)(&file_version), 1);
        input_file.read((char*)(&sample_number), 4);
        power = new float[sample_number];
        angle = new float[sample_number];
        for (int i = 0; i < sample_number; i++) {
            input_file.read((char*)(&angle[i]), 4);
            input_file.read((char*)(&power[i]), 4);
        }
    }
    input_file.close();
}

AntennaPatternFile::~AntennaPatternFile() {
    delete[] power;
    delete[] angle;
}

float AntennaPatternFile::Gain(float azimuth, float elevation) {
    int i = 0;
    while (angle[i] < azimuth)
        i = (i + 1)%sample_number;
    int i_prev = (i - 1+sample_number)%sample_number;
    return power[i_prev] + (power[i] - power[i_prev]) / (angle[i] - angle[i_prev]) * (azimuth - angle[i_prev]);
}


float AntennaPatternFile::angleMax() {
    return M_PI/4.0;
}

float AntennaPatternFile::angleMin() {
    return -1*M_PI/4.0;
}