#ifndef SIMULATOR_H
#define SIMULATOR_H

// Struct declarations if needed
struct VideoChunk {
    int chunk_id;
    int bitrate;
    int size_kb;
    int duration_ms;
    int requested_time;
    int received_time;
    int played_time;
    bool is_downloaded;
};

#endif
