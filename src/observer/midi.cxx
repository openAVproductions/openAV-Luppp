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

#include "midi.hxx"

#include <sstream>

#include "../jack.hxx"

extern Jack* jack;

MidiIO::MidiIO() :
	portsRegistered(false),
	jackInputPort(0),
	jackOutputPort(0)
{
	//LUPPP_NOTE("MidiIO %i",this);
}

MidiIO::~MidiIO()
{
	jack->unregisterMidiIO( this );

	//LUPPP_NOTE("~MidiIO unregistring ports");
	jack_port_unregister( jack->getJackClientPointer(), jackInputPort );
	jack_port_unregister( jack->getJackClientPointer(), jackOutputPort );
}


void MidiIO::writeMidi( unsigned char* data )
{
	void* portBuffer = jack_port_get_buffer( jackOutputPort, jack->getBuffersize() );

	unsigned char* buffer = jack_midi_event_reserve( portBuffer, 0, 3);
	if( buffer == 0 ) {
		return;
	} else {
		//memcpy( buffer, data, sizeof(unsigned char)*3 );
		buffer[0] = data[0];
		buffer[1] = data[1];
		buffer[2] = data[2];
	}

}

int MidiIO::registerMidiPorts(std::string name)
{
	if( !jack ) {
		LUPPP_ERROR("Attempted register of controller, JACK not instantiated yet!");
		return LUPPP_RETURN_ERROR;
	}

	jack_client_t* c = jack->getJackClientPointer();

	// register the JACK MIDI ports
	stringstream i;
	i << name << " in";
	jackInputPort  = jack_port_register( c,
	                                     i.str().c_str(),
	                                     JACK_DEFAULT_MIDI_TYPE,
	                                     JackPortIsInput,
	                                     0 );
	stringstream o;
	o << name << " out";
	jackOutputPort  = jack_port_register( c,
	                                      o.str().c_str(),
	                                      JACK_DEFAULT_MIDI_TYPE,
	                                      JackPortIsOutput,
	                                      0 );

	if ( jackInputPort && jackOutputPort ) {
		//LUPPP_NOTE("%i, %i", jackInputPort, jackOutputPort );
		portsRegistered = true;
		return LUPPP_RETURN_OK;
	} else {
		LUPPP_ERROR("Error registering JACK ports" );
		return LUPPP_RETURN_ERROR;
	}
}


void MidiIO::initBuffers(int nframes)
{
	if ( !portsRegistered )
		return;

	// clear the output buffer
	void* outputBuffer= (void*) jack_port_get_buffer( jackOutputPort, nframes );
	jack_midi_clear_buffer( outputBuffer );
}

void MidiIO::process(int nframes)
{
	if ( !portsRegistered )
		return;

	// get port buffers and setup
	void* inputBuffer = (void*) jack_port_get_buffer( jackInputPort, nframes );

	jack_midi_event_t event;
	int index = 0;
	int event_count = (int) jack_midi_get_event_count( inputBuffer );
	while ( index < event_count ) {
		jack_midi_event_get(&event, inputBuffer, index);
		midi( (unsigned char*) &event.buffer[0] );
		//printf( "MIDI %i %i %i\n", int(event.buffer[0]), int(event.buffer[1]), int(event.buffer[2]) );
		index++;
	}

}
