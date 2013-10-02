
#ifndef LUPPP_MIDI_OBSERVER_H
#define LUPPP_MIDI_OBSERVER_H

#include <iostream>
#include <string>

#include <jack/midiport.h>

class Jack;
extern Jack* jack;

/** MidiObserver
 *  A base class for obseriving a MIDI port
**/
class MidiObserver
{
  public:
    MidiObserver();
    virtual ~MidiObserver(){};
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// 
    int status();
    
    /// gets called each process() in JACK
    void process(int nframes);
    
    /// called by the subclass to create MIDI ports
    void registerMidiPorts(std::string name);
    
    virtual void midi(unsigned char* data){};
    
    void writeMidi( unsigned char* data );
  
  private:
    // there are jack_port_t* registered ports
    jack_port_t* jackInputPort;
    jack_port_t* jackOutputPort;
  
};

#endif // LUPPP_MIDI_OBSERVER_H
