
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
    MidiObserver();
    virtual ~MidiObserver(){};
    
    /// registers class with jack's MIDI handling, with MIDI port name
    void registerPorts( std::string portName );
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// gets called each process() in JACK
    virtual void process(int nframes) = 0;
  
  protected:
    // there are jack_port_t* registered ports
    void* jackInputPort;
    void* jackOutputPort;
  
};

#endif // LUPPP_MIDI_OBSERVER_H
