#ifndef ANTENNA_PATTERN_H
#define ANTENNA_PATTERN_H

#include <string>

class AntennaPattern {
public:
    virtual float Gain(float azimuth, float elevation) = 0;
    virtual float angleMax() = 0;
    virtual float angleMin() = 0;
};

class AntennaPatternAnalytical : public AntennaPattern {
public:
/*     float max_angle = M_PI/4.0;
    float min_angle = -1*M_PI/4.0; */
    float angleMax();
    float angleMin();
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
    float angleMax();
    float angleMin();
    float Gain(float azimuth, float elevation);
};

#endif