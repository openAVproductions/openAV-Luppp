
#ifndef LUPPP_CONFIG_H
#define LUPPP_CONFIG_H

///     DEBUG OPTIONS
// Track operations
#define DEBUG_TRACKS    1

// Clip selection / queueing
#define DEBUG_CLIP      1

// Buffer loading / resizing
#define DEBUG_BUFFER    1


/// General Options
#define NTRACKS 8
#define NSCENES 10
#define MAX_BUFFER_SIZE 1024
// nsamples remaining during recording before Looper requests larger buffer
#define LOOPER_SAMPLES_BEFORE_REQUEST 44100

#endif // LUPPP_CONFIG_H

