
#ifndef LUPPP_MIDI_IO_H
#define LUPPP_MIDI_IO_H

#include <iostream>
#include <string>

#include <jack/midiport.h>

class Jack;
extern Jack* jack;

/** MidiIO
 *  A base class for obseriving a MIDI port
**/
class MidiIO
{
  public:
    MidiIO();
    virtual ~MidiIO();
    
    /// name string to show in UI
    virtual std::string getName() = 0;
    
    /// gets / clears MIDI buffers
    void initBuffers(int nframes);
    
    /// gets called each process() in JACK
    void process(int nframes);
    
    /// called by the subclass to create MIDI ports
    int registerMidiPorts(std::string name);
    
    virtual void midi(unsigned char* data){};
    
    void writeMidi( unsigned char* data );
  
  private:
    // there are jack_port_t* registered ports
    jack_port_t* jackInputPort;
    jack_port_t* jackOutputPort;
  
};

#endif // LUPPP_MIDI_IO_H
