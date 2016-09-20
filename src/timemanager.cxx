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
  fpb = samplerate / 2;
  
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


int TimeManager::getFpb()
{
  return fpb;
}

void TimeManager::setBpm(float bpm)
{
#ifdef DEBUG_TIME
  LUPPP_NOTE("%s %f","setBpm()",bpm);
#endif
  setFpb( samplerate / bpm * 60 );
}

void TimeManager::setBpmZeroOne(float b)
{
  setBpm( b * 160 + 60 ); // 60 - 220
}


void TimeManager::setFpb(float f)
{
  fpb = f;
  int bpm = ( samplerate * 60) / f;
  
  char buffer [50];
  sprintf (buffer, "TM, setFpb() %i, bpm = %i", int(f), int(bpm) );
  EventGuiPrint e( buffer );
  writeToGuiRingbuffer( &e );
  
  EventTimeBPM e2( bpm );
  writeToGuiRingbuffer( &e2 );
  
  for(uint i = 0; i < observers.size(); i++)
  {
    observers.at(i)->setFpb(fpb);
  }
}

void TimeManager::registerObserver(TimeObserver* o)
{
  //LUPPP_NOTE("%s","registerObserver()");
  observers.push_back(o);
  o->setFpb( fpb );
  
  int bpm = ( samplerate * 60) / fpb;
  EventTimeBPM e2( bpm );
  writeToGuiRingbuffer( &e2 );
}

void TimeManager::tap()
{
  // reset tap tempo to "first tap" if more than 5 secs elapsed since last tap
  if ( tapTempo[0] < totalFrameCounter - samplerate * 5 )
  {
    tapTempoPos = 0;
  }
  
  if ( tapTempoPos < 3 )
  {
    tapTempo[tapTempoPos] = totalFrameCounter;
    tapTempoPos++;
  }
  else
  {
    // calculate frames per tap
    int tapFpb1 = tapTempo[1] - tapTempo[0];
    int tapFpb2 = tapTempo[2] - tapTempo[1];
    int tapFpb3 = totalFrameCounter - tapTempo[2]; // last tap, until now
    
    int average = (tapFpb1 + tapFpb2 + tapFpb3) / 3;

    if( average < 13000 )
    {
	    char buffer [50];
	    sprintf (buffer, "TM, tap() average too slow! quitting");
	    EventGuiPrint e( buffer );
	    writeToGuiRingbuffer( &e );
	    return;
    }
    
    char buffer [50];
    sprintf (buffer, "TM, tap() average = %i", average );
    EventGuiPrint e( buffer );
    writeToGuiRingbuffer( &e );
    

    setFpb(average);
    
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
}

void TimeManager::process(Buffers* buffers)
{
  // time signature?
  //buffers->transportPosition->beats_per_bar = 4;
  //buffers->transportPosition->beat_type     = 4;
  
  if ( transportState == TRANSPORT_STOPPED )
  {
    return;
  }
  
  int nframes = buffers->nframes;
  


  
  if ( beatFrameCountdown < nframes )
  {
      //length of beat is not multiple of nframes, so need to process last frames *before*
      //then set beat (get the queued actions: play, rec etc)
      // then process first frames *after* beat
      int before=(beatCounter*fpb)%nframes;
      int after=nframes-before;
    
    if ( before < nframes && after <= nframes && before + after == nframes )
    {
      char buffer [50];
//      sprintf (buffer, "Timing OK: before %i, after %i, b+a %i",  before, after, before+after );
//      EventGuiPrint e2( buffer );
//      writeToGuiRingbuffer( &e2 );
      
    }
    else
    {
      char buffer [50];
      sprintf (buffer, "Timing Error: before: %i, after %i", before, after );
      EventGuiPrint e2( buffer );
      writeToGuiRingbuffer( &e2 );
    }
    
    // process before beat:
    jack->processFrames( before );
    
    // handle beat:
    // inform observers of new beat FIRST
    for(uint i = 0; i < observers.size(); i++)
    {
      observers.at(i)->beat();
    }
    
    if ( beatCounter % 4 == 0 )
    {
      // inform observers of new bar SECOND
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->bar();
      }
      barCounter++;
      //beatCounter=0;
    }
    
    // process after
    jack->processFrames( after );
    
    // write new beat to UI (bar info currently not used)
    EventTimeBarBeat e( barCounter, beatCounter );
    writeToGuiRingbuffer( &e );
    
    
    
    beatFrameCountdown = fpb-after;
    beatCounter++;
  }
  else
  {
    jack->processFrames( nframes );
    beatFrameCountdown -= nframes;

  }

  totalFrameCounter += nframes;
  
  // write BPM / transport info to JACK
  int bpm = ( samplerate * 60) / fpb;
  if ( buffers->transportPosition )
  {
    buffers->transportPosition->valid = (jack_position_bits_t)(JackPositionBBT | JackTransportPosition);
    
    buffers->transportPosition->bar  = beatCounter / 4;
    buffers->transportPosition->beat = (beatCounter % 4) + 1; // beats 1-4
    
    float part = float( fpb-beatFrameCountdown) / fpb;
    buffers->transportPosition->tick = part > 1.0f? 0.9999*1920 : part*1920;
    
    buffers->transportPosition->frame = totalFrameCounter;
    
    buffers->transportPosition->ticks_per_beat = 1920;
    buffers->transportPosition->beats_per_bar = 4;
    
    buffers->transportPosition->beats_per_minute = bpm;
  }
}

