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

#include "timemanager.hxx"

#include <iostream>
#include <cstdio>

#ifdef DEBUG_TIME
	#include <iomanip>
#endif

#include "buffers.hxx"
#include "eventhandler.hxx"

#include "observer/time.hxx"

#include "jack.hxx"

extern Jack* jack;

using namespace std;

TimeManager::TimeManager():
	transportState( TRANSPORT_ROLLING ),
	observers()
{
	samplerate = jack->getSamplerate();
	// 120 BPM default
	_fpb = samplerate / 2;

	//Counter for current bar/beat
	barCounter  = 0;
	beatCounter = 0;

	previousBeat = 0;

	//In process() we want to immediately process bar(), beat() of all observers
	// thats why beatFrameCountdown<nframes, but we don't know yet what value nframes has
	// so set beatFrameCountdown to a value that garantees beatFrameCountdown<nframes
	beatFrameCountdown = -1;//fpb;

	totalFrameCounter = 0;

	tapTempoPos = 0;
	tapTempo[0] = 0;
	tapTempo[1] = 0;
	tapTempo[2] = 0;
}


double TimeManager::getFpb()
{
	return _fpb;
}

void TimeManager::queueBpmChange(float bpm)
{
	double frames = (double)samplerate * (double)60 / (double)bpm;
#ifdef DEBUG_TIME
	LUPPP_NOTE("%s %f","setBpm()",bpm);
	cout << "calculated: " << fixed << setprecision(20) << frames << "\n";
#endif
	queueFpbChange( frames );
}

void TimeManager::queueBpmChangeZeroOne(float b)
{
	queueBpmChange( b * (MAX_TEMPO - MIN_TEMPO) + MIN_TEMPO ); // MIN_TEMPO - MAX_TEMPO
}

void TimeManager::queueFpbChange( double f )
{
	float bpm = (samplerate * 60.f) / f;
	if(bpm < MIN_TEMPO || bpm > MAX_TEMPO) {
		char buffer[128];
		snprintf(buffer, sizeof(buffer), "drop TM::qfpb() %d bpm = %d",
			 (int)f, (int)bpm);
		EventGuiPrint e( buffer );
		writeToGuiRingbuffer( &e );
		return;
	}

	_bpmChangeQueued = true;
	_nextFpb = f;
}

void TimeManager::setFpb(double f)
{
	// allign beatFrameCountdown
	long double ratio = (long double)f / (long double)_fpb;
	beatFrameCountdown = beatFrameCountdown * ratio;

	_fpb = f;
	int bpm = ( samplerate * 60) / f;

	char buffer [50];
	snprintf(buffer, sizeof(buffer), "TM, setFpb() %i, bpm = %i",
		  int(f), int(bpm) );
	EventGuiPrint e( buffer );
	writeToGuiRingbuffer( &e );

	EventTimeBPM e2( bpm );
	writeToGuiRingbuffer( &e2 );

	for(uint i = 0; i < observers.size(); i++) {
		observers.at(i)->setFpb(_fpb);
	}
}

void TimeManager::registerObserver(TimeObserver* o)
{
	//LUPPP_NOTE("%s","registerObserver()");
	observers.push_back(o);
	o->setFpb( _fpb );

	int bpm = ( samplerate * 60) / _fpb;
	EventTimeBPM e2( bpm );
	writeToGuiRingbuffer( &e2 );
}

void TimeManager::tap()
{
	// reset tap tempo to "first tap" if more than 5 secs elapsed since last tap
	if ( tapTempo[0] < totalFrameCounter - samplerate * 5 ) {
		tapTempoPos = 0;
	}

	if ( tapTempoPos < 3 ) {
		tapTempo[tapTempoPos] = totalFrameCounter;
		tapTempoPos++;
	} else {
		// calculate frames per tap
		int tapFpb1 = tapTempo[1] - tapTempo[0];
		int tapFpb2 = tapTempo[2] - tapTempo[1];
		int tapFpb3 = totalFrameCounter - tapTempo[2]; // last tap, until now

		int average = (tapFpb1 + tapFpb2 + tapFpb3) / 3;

		if( average < 13000 ) {
			char buffer [50];
			snprintf(buffer, sizeof(buffer),
				 "TM, tap() average too slow! quitting");
			EventGuiPrint e( buffer );
			writeToGuiRingbuffer( &e );
			return;
		}

		char buffer [50];
		snprintf(buffer, sizeof(buffer), "TM, tap() average = %i",
			 average);
		EventGuiPrint e( buffer );
		writeToGuiRingbuffer( &e );


		queueFpbChange(average);

		// reset, so next 3 taps restart process
		tapTempoPos = 0;
	}
}

int TimeManager::getNframesToBeat()
{
	// FIXME
	return -1; //beatFrameCountdown;
}

void TimeManager::setTransportState( TRANSPORT_STATE s )
{
	transportState = s;
	if(transportState == TRANSPORT_STOPPED)
		jack->transportRolling(false);
	else {
		jack->transportRolling(true);
		barCounter  = 0;
		beatCounter = 0;
		beatFrameCountdown = -1;
		for(int i=0; i<observers.size(); i++)
			observers[i]->resetTimeState();
	}
}

void TimeManager::process(Buffers* buffers)
{
	if(_bpmChangeQueued) {
		setFpb(_nextFpb);
		_bpmChangeQueued = false;
	}
	// time signature?
	//buffers->transportPosition->beats_per_bar = 4;
	//buffers->transportPosition->beat_type     = 4;

	if ( transportState == TRANSPORT_STOPPED ) {
		return;
	}

	int nframes = buffers->nframes;




	if ( beatFrameCountdown < nframes ) {
		//length of beat is not multiple of nframes, so need to process last frames of last beat *before* setting next beat
		//then set new beat (get the queued actions: play, rec etc)
		// then process first frames *after* new beat
		int before=int(beatCounter*_fpb)%nframes;
		int after=nframes-before;

		if ( before < nframes && after <= nframes && before + after == nframes ) {
			char buffer [50];
//      snprintf(buffer, sizeof(buffer), "Timing OK: before %i, after %i, b+a %i",
//               before, after, before+after );
//      EventGuiPrint e2( buffer );
//      writeToGuiRingbuffer( &e2 );

		} else {
			char buffer [50];
			snprintf(buffer, sizeof(buffer),
				 "Timing Error: before: %i, after %i",
				 before, after );
			EventGuiPrint e2( buffer );
			writeToGuiRingbuffer( &e2 );
		}

		// process before beat:
		if(before)
			jack->processFrames( before );

		// handle beat:
		// inform observers of new beat FIRST
		for(uint i = 0; i < observers.size(); i++) {
			observers.at(i)->beat();
		}

		if ( beatCounter % 4 == 0 ) {
			// inform observers of new bar SECOND
			for(uint i = 0; i < observers.size(); i++) {
				observers.at(i)->bar();
			}
			barCounter++;
			//beatCounter=0;
		}

		// process after
		// we need to clear internal buffers in order to write *after* frames to them
		jack->clearInternalBuffers(nframes);
		if(after)
			jack->processFrames( after );

		// write new beat to UI (bar info currently not used)
		EventTimeBarBeat e( barCounter, beatCounter );
		writeToGuiRingbuffer( &e );



		beatFrameCountdown = _fpb-after;
		beatCounter++;
	} else {
		jack->processFrames( nframes );
		beatFrameCountdown -= nframes;

	}

	totalFrameCounter += nframes;

	// write BPM / transport info to JACK
	int bpm = ( samplerate * 60) / _fpb;
	if ( buffers->transportPosition ) {
		buffers->transportPosition->valid = (jack_position_bits_t)(JackPositionBBT | JackTransportPosition);

		buffers->transportPosition->bar  = beatCounter / 4 + 1;// bars 1-based
		buffers->transportPosition->beat = (beatCounter % 4) + 1; // beats 1-4

		float part = float( _fpb-beatFrameCountdown) / _fpb;
		buffers->transportPosition->tick = part > 1.0f? 0.9999*1920 : part*1920;

		buffers->transportPosition->frame = totalFrameCounter;

		buffers->transportPosition->ticks_per_beat = 1920;
		buffers->transportPosition->beats_per_bar = 4;

		buffers->transportPosition->beats_per_minute = bpm;
	}
}

