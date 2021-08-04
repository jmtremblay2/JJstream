#ifndef MPEGTS_FEATURES
#define MPEGTS_FEATURES

void discover_tables(mpegts_reader_data* rd, const char* output_folder);
void find_telemetry(mpegts_reader_data* rd, const char* output_folder, uint32_t telemetry_pid);

#endif