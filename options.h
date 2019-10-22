#ifndef OPTIONS_H
#define OPTIONS_H


/* struct options_t
 * Contains all the options for the simulator.
*/
typedef struct options_t {
    float       DEM_PARSER_DELTA_DISTANCE;
    uint8_t     DEM_PARSER_EXPORT_ELEVATION_MAP;
    uint8_t     DEM_PARSER_EXPORT_AZIMUTH_ANGLE;
    uint8_t     DEM_PARSER_EXPORT_ELEVATION_ANGLE;
    uint8_t     DEM_PARSER_EXPORT_GRAZING_ANGLE;
    uint8_t     DEM_PARSER_EXPORT_SHADOWING;

    
    float       SIMULATOR_RADIUS;
    uint8_t     SIMULATOR_SHADOWING_ENABLED;
    float       SIMULATOR_TRANSMIT_POWER; // Power in watts.
    float       SIMULATOR_TRANSMIT_FREQUENCY;
    float       SIMULATOR_TRANSMIT_PULSE_LENGTH;
    uint16_t    SIMULATOR_RANGE_BIN_COUNT;
    uint16_t    SIMULATOR_AZIMUTH_ANGLE_COUNT;
    float       SIMULATOR_WAVE_SPEED;
} options_t;

#endif
