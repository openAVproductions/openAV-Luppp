
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
    /// registers class with jack's MIDI handling, with MIDI port name
    MidiObserver( std::string portName );
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// gets called with each MIDI message from the controller. Deal with its
    /// input here, and route to the appropriate function in Luppp
    virtual void midi(unsigned char* data) = 0;
    
    /// sets the port number for this instance. Used when writing MIDI messages
    /// to identify the port, using Jack::midiObserverWriteMIDI()
    void port(int index);
    
    /// returns the port index to the subclass when needed
    int port();
  
  private:
    int portIndex;
  
};

#endif // LUPPP_MIDI_OBSERVER_H
