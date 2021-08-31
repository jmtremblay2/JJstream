#include <inttypes.h>
#include <stdio.h>
#include "mpegts_packet.h"
#include "mpegts_pat.h"


typedef struct misb_telemetry_raw {
    uint16_t Checksum; //1
    uint64_t UNIX_Time_Stamp; //2
    uint16_t Platform_Heading_Angle; //5
    int16_t Platform_Pitch_Angle; //6
    int16_t Platform_Roll_Angle; //7
    int32_t Sensor_Latitude; //13
    int32_t Sensor_Longitude; //14
    uint16_t Sensor_True_Altitude; // 15
    uint16_t Sensor_Horizontal_Field_of_View; // 16
    uint16_t Sensor_Vertical_Field_of_View; // 17
    uint32_t Sensor_Relative_Azimuth_Angle; // 18
    int32_t Sensor_Relative_Elevation_Angle; // 19
    uint32_t Sensor_Relative_Roll_Angle; // 20
} misb_telemetry_raw;

typedef struct misb_telemetry_formatted {
    uint16_t Checksum; //1
    uint64_t UNIX_Time_Stamp; //2
    double Platform_Heading_Angle; //5
    double Platform_Pitch_Angle; //6
    double Platform_Roll_Angle; //7
    double Sensor_Latitude; //13
    double Sensor_Longitude; //14
    double Sensor_True_Altitude; // 15
    double Sensor_Horizontal_Field_of_View; // 16
    double Sensor_Vertical_Field_of_View; // 17
    double Sensor_Relative_Azimuth_Angle; // 18
    double Sensor_Relative_Elevation_Angle; // 19
    double Sensor_Relative_Roll_Angle; // 20
} misb_telemetry_formatted;

misb_telemetry_raw get_raw_telemetry(ts_packet* ts);
misb_telemetry_formatted misb_raw_to_formatted(misb_telemetry_raw* mr);
void print_misb_raw(misb_telemetry_raw* mr);
void print_misb_formatted(misb_telemetry_formatted* mf);

void fprintf_misb_formatted_header(FILE* fout);
void fprintf_misb_formatted(FILE* fout, misb_telemetry_formatted* mf);