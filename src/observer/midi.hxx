
#ifndef LUPPP_MIDI_OBSERVER_H
#define LUPPP_MIDI_OBSERVER_H

#include <iostream>
#include <string>

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
    
    
    
    /// gets called each process() in JACK
    void process(int nframes);
    
    /// called by the subclass to create MIDI ports
    void registerMidiPorts(std::string name);
    
    virtual void midi(unsigned char* data){};
  
  private:
    // there are jack_port_t* registered ports
    void* jackInputPort;
    void* jackOutputPort;
  
};

#endif // LUPPP_MIDI_OBSERVER_H
