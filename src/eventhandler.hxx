
#ifndef LUPPP_EVENT_HANDLER_H
#define LUPPP_EVENT_HANDLER_H

#include "event.hxx"
#include <jack/ringbuffer.h>

extern char* processDspMem;
extern char* processOscMem;

extern jack_ringbuffer_t* rbToDsp;
//extern jack_ringbuffer_t* ringbufferDspToGui;

// to process events in the ringbuffer
extern void handleDspEvents();
//extern void handleGuiEvents();

// to write events to the ringbuffer
extern void writeToDspRingbuffer(EventBase* e);
//extern void writeToGuiRingbuffer(EventBase* e);

#endif // LUPPP_EVENT_HANDLER_H

