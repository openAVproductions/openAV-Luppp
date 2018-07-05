/*
 * Author: Gerald Mwangi 2016
 *         gerald.mwangi@gmx.de
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

#ifndef JACKSENDRETURN_H
#define JACKSENDRETURN_H

#include "buffers.hxx"
#include "audioprocessor.hxx"
#include <jack/jack.h>

/// JackSendReturn: This class provides per track send and return jack ports
/// The send port always sends the output of the previous AudioProcesser
/// The content of the return channel depends on whether the return jack port is connected:
/// If it is connected it receives what ever audio comes into it.
/// If it is not connected, it receives the output of the previous AudioProcessor (it gets what goes into the send port)
class JackSendReturn: public AudioProcessor
{
public:
	//Constructor: the registration of the jack send/return is done here
	JackSendReturn(int trackid,AudioProcessor* prev,jack_client_t* client);

	//The process callback
	virtual void process(unsigned int nframes, Buffers* buffers);

	//Activate the return chain. When _active=true then Buffers::RETURN_TRACK_0+_trackid gets the data
	//from the return port. The send port always send the incoming data
	void activate(bool act);
	void sendVolume(float vol);

private:
	bool _active;
	float _sendVol;
	float _sendVolLag;

	jack_port_t* _sendPortL;
	jack_port_t* _sendPortR;
	jack_port_t* _returnPortL;
	jack_port_t* _returnPortR;
	int _trackId;
	AudioProcessor* _previousProcessor;
	int _counter;
};

#endif
