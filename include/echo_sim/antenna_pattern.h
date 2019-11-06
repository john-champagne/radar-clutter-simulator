#ifndef ANTENNA_PATTERN_H
#define ANTENNA_PATTERN_H

#include <string>

class AntennaPattern {
public:
    virtual float Gain(float azimuth, float elevation) = 0;
};

class AntennaPatternAnalytical : public AntennaPattern {
public:
    float Gain(float azimuth, float elevation);
};

class AntennaPatternFile : public AntennaPattern {
private:
    float* power;
    float* angle;
    uint32_t sample_number = 0;
public:
    AntennaPatternFile(std::string filename);
    ~AntennaPatternFile();    
    float Gain(float azimuth, float elevation);
};

#endif