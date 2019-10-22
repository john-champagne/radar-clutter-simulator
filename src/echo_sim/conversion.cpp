#include <math.h>
#include "echo_sim/clutter_coefficient.H"
#define MIN_DBM -200
double dBmToWatt(float dBm)  { return pow(10, dBm/10.0) / 1000.0; }
double dBToGain(float dB)  { return pow(10, dB/10.0); }
double WattTodBm(float Watt) { return Watt <= 0 ? MIN_DBM : log10(Watt*1000)*10; }

