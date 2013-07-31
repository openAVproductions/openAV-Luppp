
#ifndef LUPPP_MIDI_OBSERVER_H
#define LUPPP_MIDI_OBSERVER_H

#include <iostream>
#include <string>

class Jack;
extern Jack* jack;

/** MidiObserver
 *  A base class allowing the recieving of MIDI message streams to the subclass
**/
class MidiObserver
{
  public:
    /// registers class with jack's MIDI handling, with optional MIDI port name
    MidiObserver( std::string portName = "" );
    
    /// gets called with each MIDI message from the controller. Deal with its
    /// input here, and route to the appropriate function in Luppp
    virtual void midi(unsigned char* data) = 0;
};

#endif // LUPPP_MIDI_OBSERVER_H
