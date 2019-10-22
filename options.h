#ifndef OPTIONS_H
#define OPTIONS_H


/* struct options_t
 * Contains all the options for the simulator.
*/
typedef struct options_t {
    float       DEM_PARSER_DELTA_DISTANCE = 30.0;
    uint8_t     DEM_PARSER_EXPORT_ELEVATION_MAP = 0;
    uint8_t     DEM_PARSER_EXPORT_AZIMUTH_ANGLE = 0;
    uint8_t     DEM_PARSER_EXPORT_ELEVATION_ANGLE = 0;
    uint8_t     DEM_PARSER_EXPORT_GRAZING_ANGLE = 0;
    uint8_t     DEM_PARSER_EXPORT_SHADOWING = 0;
    
    float       SIMULATOR_RADIUS = 264000.0;
    uint8_t     SIMULATOR_SHADOWING_ENABLED = 1;
    float       SIMULATOR_TRANSMIT_POWER = 400000;
    float       SIMULATOR_TRANSMIT_FREQUENCY = 200000;
    float       SIMULATOR_TRANSMIT_PULSE_LENGTH = 43.3333333E-6;
    uint16_t    SIMULATOR_RANGE_BIN_COUNT = 540;
    uint16_t    SIMULATOR_AZIMUTH_ANGLE_COUNT = 4096;
    float       SIMULATOR_WAVE_SPEED = 299792458;

    int8_t      SIMULATOR_THREAD_COUNT = -1; // -1 indicates that the program will decide.
                                             // If you have more than 127 threads:
                                             // First of all, congrats.
                                             // Second, you'll need to make this a int16_t
} options_t;

#endif
