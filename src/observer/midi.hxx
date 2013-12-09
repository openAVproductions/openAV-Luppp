/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    
    virtual void midi(unsigned char* /*data*/){};
    
    void writeMidi( unsigned char* /*data*/ );
  
  private:
    bool portsRegistered;
    // there are jack_port_t* registered ports
    jack_port_t* jackInputPort;
    jack_port_t* jackOutputPort;
  
};

#endif // LUPPP_MIDI_IO_H
