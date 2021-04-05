#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mpegts_header.h"
#include "mpegts_packet.h"
#include "mpegts_pat.h"
#include "args.h"

#define BUFFER_MAX 1000000
#define READ_MORE 500
#define PAT_PID 0
typedef struct misb_telemetry_raw {
    uint16_t Checksum; //1
    uint64_t UNIX_Time_Stamp; //2
    int16_t Platform_Heading_Angle; //5
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

misb_telemetry_formatted misb_raw_to_formatted(misb_telemetry_raw* mr){
    misb_telemetry_formatted mf;
    mf.Platform_Heading_Angle = ((double)mr->Platform_Heading_Angle) / UINT16_MAX;
    mf.Platform_Pitch_Angle = ((double)mr->Platform_Pitch_Angle) / INT16_MAX * 20.0;
    mf.Platform_Roll_Angle = ((double)mr->Platform_Roll_Angle) / INT16_MAX * 50.0;
    mf.Sensor_Latitude = ((double) mr->Sensor_Latitude) / INT32_MAX * 90;
    mf.Sensor_Longitude = ((double) mr->Sensor_Longitude) / INT32_MAX * 180;
    mf.Sensor_True_Altitude = (((double) mr->Sensor_True_Altitude) / UINT16_MAX) * 19900 - 900;
    mf.Sensor_Horizontal_Field_of_View = ((double) mr->Sensor_Horizontal_Field_of_View) / UINT16_MAX * 180;
    mf.Sensor_Vertical_Field_of_View = ((double) mr->Sensor_Vertical_Field_of_View) / UINT16_MAX * 180;
    mf.Sensor_Relative_Azimuth_Angle = ((double) mr->Sensor_Relative_Azimuth_Angle) / UINT32_MAX * 360;
    mf.Sensor_Relative_Elevation_Angle = ((double) mr->Sensor_Relative_Elevation_Angle) / INT32_MAX * 180;
    mf.Sensor_Relative_Roll_Angle = ((double) mr->Sensor_Relative_Roll_Angle) / UINT32_MAX * 360;
    return mf;
}

void print_misb_raw(misb_telemetry_raw* mr){
    printf("raw Platform Heading Angle: %u\n", mr->Platform_Heading_Angle);    
    return;
}

void print_misb_formatted(misb_telemetry_formatted* mf){
    printf("Platform Heading Angle: %lf\n", mf->Platform_Heading_Angle);
    printf("Platform Pitch Angle: %lf\n", mf->Platform_Pitch_Angle);
    printf("Platform Roll Angle: %lf\n", mf->Platform_Roll_Angle);
    printf("Sensor Latitude: %lf\n", mf->Sensor_Latitude);
    printf("Sensor Longitude: %lf\n", mf->Sensor_Longitude);
    printf("Sensor True Altitude: %lf\n", mf->Sensor_True_Altitude);
    printf("Sensor Horizontal fov: %lf\n", mf->Sensor_Horizontal_Field_of_View);
    printf("sensor Vertical fov: %lf\n", mf->Sensor_Vertical_Field_of_View);
    printf("Sensor Relative Azimuth: %lf\n", mf->Sensor_Relative_Azimuth_Angle);
    printf("Sensor Relative Elevation: %lf\n", mf->Sensor_Relative_Elevation_Angle);
    printf("Sensor Relative Roll: %lf\n", mf->Sensor_Relative_Roll_Angle);
    return;
}

void copy_n_bytes_reverse(uint8_t* dest, uint8_t* source, size_t n){
    for(int i = 0; i < n; ++i)
        dest[i] = source[n-i];
}

uint16_t read_uint16(uint8_t *data){
    uint16_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 2);
    return x;
}

int16_t  read_int16(uint8_t *data){
    int16_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 2);
    return x;
}

uint32_t read_uint32(uint8_t *data){
    uint32_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 4);
    return x;
}

int32_t  read_int32(uint8_t *data){
    int32_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 4);
    return x;
}

uint64_t read_uint64(uint8_t *data){
    uint64_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 8);
    return x;
}

int64_t  read_int64(uint8_t *data){
    uint64_t x;
    copy_n_bytes_reverse((uint8_t*) &x, data, 8);
    return x;

}

uint8_t misb_key[] = {0x06, 0x0E, 0x2B, 0x34, 
                      0x02, 0x0B, 0x01, 0x01, 
                      0x0E, 0x01, 0x03, 0x01,
                      0x01, 0x00, 0x00, 0x00};

mpegts_pat* find_pat(mpegts_reader_data* rd){
    ts_packet ts;
    while(has_next_ts_packet(rd)){
        get_next_ts_packet(rd, &ts);
        if(ts.h.pid == PAT_PID){
            mpegts_pat* p = read_pat(&ts);
            return p;
        }
    }
    fprintf(stderr, "could not find PAT\n");
    return NULL;
}

mpegts_pmt* find_pmt(mpegts_reader_data* rd, uint16_t pmt_pid){
    ts_packet ts;
    while(has_next_ts_packet(rd)){
        get_next_ts_packet(rd, &ts);
        if(ts.h.pid == pmt_pid){
            mpegts_pmt *pmt = read_pmt(&ts);
            return pmt;
        }
    }
    fprintf(stderr, "could not find PMT\n");
    return NULL;
}

int contains_uint16(uint16_t *list, uint16_t value, size_t size){
    for(int i = 0; i < size; ++i)
        if(value == list[i])
            return 1;
    return 0;
}

int compare_uint8_memory(uint8_t *a, uint8_t* b, size_t size){
    for(int i = 0; i < size; ++i){
        if(a[i] != b[i])
            return 1;
    }
    return 0;
}

uint16_t find_klv_pid(mpegts_reader_data* rd, mpegts_pmt* pmt){
    uint16_t stream_pids[pmt->num_streams];
    for(int i = 0; i < pmt->num_streams; ++i)
        stream_pids[i] = pmt->streams[i].elementary_pid;
    ts_packet ts;
    while(has_next_ts_packet(rd)){
        get_next_ts_packet(rd, &ts);
        uint16_t pid = ts.h.pid;
        int pid_in_streams_pid = contains_uint16(stream_pids, pid, pmt->num_streams);
        if(pid_in_streams_pid){
            //print_ts_packet(&ts, 0, 0);
            for(int i = 0; i < 100; ++i){
                int cmp = compare_uint8_memory(ts.payload + i, misb_key, 16);//memcmp(ts.payload + i, misb_key, 16);
                if(0 == cmp){
                    print_ts_packet(&ts, 0, 0);
                return pid;
                }
            }
        }
    }
    return 0;
}

void parse_klv_basic(ts_packet* ts){
    //uint8_t *data = ts->payload;
    int i = 0;
    for(i = 0; i < 100; ++i)
        if(0 == compare_uint8_memory(ts->payload + i, misb_key, 16))
            break;
    if(i == 100)
        return;
    uint8_t *data = ts->payload + i + 16 + 2;
    //print_ts_packet(ts, 0, 0);

    uint8_t *last = ts->raw_data + 188;

    printf("found telemetry:\n");
    misb_telemetry_raw mr;
    do {
        uint8_t tag = data[0];
        uint8_t length = data[1];
        switch(tag){
            case 1:
                mr.Checksum = read_uint16(data + 2);
                break;
            case 2:
                mr.UNIX_Time_Stamp = read_uint64(data + 2);
                break;
            case 5:
                mr.Platform_Heading_Angle = read_uint16(data + 2);
                break;
            case 6:
                mr.Platform_Pitch_Angle = read_int16(data + 2);
                break;
            case 7:
                mr.Platform_Roll_Angle = read_int16(data + 2);
                break;
            case 13:
                mr.Sensor_Latitude = read_int32(data + 2);
                break;
            case 14:
                mr.Sensor_Longitude = read_int32(data + 2);
                break;
            case 15:
                mr.Sensor_True_Altitude = read_uint16(data + 2);
                break;
            case 16:
                mr.Sensor_Horizontal_Field_of_View = read_uint16(data + 2);
                break;
            case 17:
                mr.Sensor_Vertical_Field_of_View = read_uint16(data + 2);
                break;
            case 18:
                mr.Sensor_Relative_Azimuth_Angle = read_uint32(data + 2);
                break;
            case 19:
                mr.Sensor_Relative_Elevation_Angle = read_int32(data + 2);
                break;
            case 20:
                mr.Sensor_Relative_Roll_Angle = read_uint32(data + 2);
                break;
            default:
                break;
        }
        //printf("tag: %d, length: %d, value: ", tag, length);
        //for(int j = 0; j < length; ++j)
        //    printf("%02X",data[2+j]);
        //printf("\n");
        data += (2 + length);
    } while(data < last);


    print_misb_raw(&mr);
    misb_telemetry_formatted mf = misb_raw_to_formatted(&mr);
    print_misb_formatted(&mf);


}

void klv_loop(mpegts_reader_data* rd, uint16_t klv_pid){
    ts_packet ts;
    while(has_next_ts_packet(rd)){
        get_next_ts_packet(rd, &ts);
        if(ts.h.pid == klv_pid){
            parse_klv_basic(&ts);
        }
    }

}


int main(int argc, char** argv){
    jjstream_args args = get_args(argc, argv);

    mpegts_reader_data rd;
    ts_packet ts;
    init_mpegts_reader_data(args.fname, &rd, BUFFER_MAX, READ_MORE);

    mpegts_pat *pat = find_pat(&rd);
    print_pat(pat);
    uint16_t pmt_pid = pat->programs[0].data;

    mpegts_pmt *pmt = find_pmt(&rd, pmt_pid);
    print_pmt(pmt);

    uint16_t klv_pid = find_klv_pid(&rd, pmt);
    klv_loop(&rd, klv_pid);
    close_mpegts_reader_data(&rd);
    return 0;
}
   
//     while(has_next_ts_packet(&rd)){

//         get_next_ts_packet(&rd, &ts);
//         //print_ts_packet(&ts, 0, 0);        
        
//         if(ts.h.pid == PAT_PID){
//             //print_ts_packet(&ts, 0, 0);
//             mpegts_pat* p = read_pat(&ts);
//             if(! printed_pat){
//                 printed_pat = 1;
//                 printf("**I found the PAT:\n");
//                 print_pat(p);

//                 pmt_pid = p->programs[0].data;
//                 printf("**using pat PID: %x\n", pmt_pid);
//             }
//             delete_pat(p);
//         }
//         if(ts.h.pid == pmt_pid){
//             if(! printed_pmt){
//                 //printed_pmt = 1;
//                 printf("**I found the PMT:\n");
//                 //print_ts_packet(&ts, 0, 0);
//                 mpegts_pmt *pmt = read_pmt(&ts);
//                 print_pmt(pmt);
//                 delete_pmt(pmt);

//                 for(int i = 0; i < pmt->num_streams; ++i){
//                     uint8_t stream_type = pmt->streams[i].stream_type;
//                     if(stream_type == 6)
//                         klv_pid = pmt->streams[i].elementary_pid;
//                 }

//                 printf("using the following pid for KLV: %x\n", klv_pid);
//             }
//         }
//         // if(ts.h.pid == klv_pid){
//         //     printf("**I found the folowing telemetry:\n");
//         //     print_ts_packet(&ts, 0, 0);
//         //     break;
//         // }

//     }
//     close_mpegts_reader_data(&rd);
//     return 0;
// }

