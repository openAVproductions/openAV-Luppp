
#ifndef LUPPP_CONFIG_H
#define LUPPP_CONFIG_H


///     PROGAM WIDE DEFINES
#define NAME "Luppp"



///     DEBUG
// Track operations
//#define DEBUG_TRACKS            1

// Clip selection / queueing
//#define DEBUG_CLIP              1

// Buffer loading / resizing
//#define DEBUG_BUFFER            1

// Logic : Warning NON RT!
//#define DEBUG_LOGIC             1

// State save / load
//#define DEBUG_STATE             1

// Jack print controller out MIDI
//#define DEBUG_MIDI              1


///     GENERAL
#define NTRACKS 8
#define NSCENES 10
#define MAX_BUFFER_SIZE 1024

// nsamples remaining during recording before Looper requests larger buffer
#define LOOPER_SAMPLES_BEFORE_REQUEST 44100
#define LOOPER_SAMPLES_UPDATE_SIZE    44100

#define LUPPP_RETURN_OK       0
#define LUPPP_RETURN_WARNING  1
#define LUPPP_RETURN_ERROR    2


///     debug.hxx for printing convienience
#include "debug.hxx"


#endif // LUPPP_CONFIG_H

