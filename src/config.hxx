
#ifndef LUPPP_CONFIG_H
#define LUPPP_CONFIG_H

/// Build options
//#define DEBUG_ALL 1


#ifdef DEBUG_ALL

#define DEBUG_CLIP      1
#define DEBUG_BUFFER    1


#endif

/// General Options
#define NTRACKS 8
#define NSCENES 10
#define MAX_BUFFER_SIZE 1024

#define LOOPER_SAMPLES_BEFORE_REQUEST 44100

#endif // LUPPP_CONFIG_H

