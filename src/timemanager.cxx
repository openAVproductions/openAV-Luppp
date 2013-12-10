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
    observers()
{
  samplerate = jack->getSamplerate();
  // 120 BPM default
  fpb = samplerate / 2;
  
  barCounter  = 0;
  beatCounter = 0;
  
  beatFrameCountdown = fpb;
  
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
  return beatFrameCountdown;
}

void TimeManager::process(Buffers* buffers)
{
  // tap tempo measurements
  frame = buffers->transportFrame;
  
  // time signature?
  //buffers->transportPosition->beats_per_bar = 4;
  //buffers->transportPosition->beat_type     = 4;
  
  totalFrameCounter += buffers->nframes;
  
  // calculate beat / bar position in nframes
  int beat = totalFrameCounter / fpb;
  beatFrameCountdown = totalFrameCounter - (beat*fpb);
  
  
  if ( beatFrameCountdown < buffers->nframes )
  {
    beatCounter++;
    
    // process *upto* beat frame:
    jack->processFrames( beatFrameCountdown );
    
    // inform observers of new beat FIRST
    for(uint i = 0; i < observers.size(); i++)
    {
      observers.at(i)->beat();
    }
    
    // FIXME: 4 == beats in time sig
    if ( beatCounter % 4 == 0 )
    {
      // inform observers of new bar SECOND
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->bar();
      }
      barCounter++;
      //buffers->transportPosition->bar++;
    }
    
    // process frames after beat()
    int remaining = long(buffers->nframes) - beatFrameCountdown;
    /*
    char buffer [50];
    sprintf (buffer, "r: %i, nfr: %i, bFC %li\ttFC %lli",  remaining, buffers->nframes, beatFrameCountdown, totalFrameCounter );
    EventGuiPrint e2( buffer );
    writeToGuiRingbuffer( &e2 );
    */
    if ( remaining > 0 )
    {
      jack->processFrames( remaining );
    }
    else
    {
      char buffer [50];
      sprintf (buffer, "Timing Error: negative samples %i",  remaining );
      EventGuiPrint e2( buffer );
      writeToGuiRingbuffer( &e2 );
    }
    
    // write new beat to UI (bar info currently not used)
    EventTimeBarBeat e( barCounter, beatCounter );
    writeToGuiRingbuffer( &e );
    
    beatFrameCountdown = fpb; 
  }
  else
  {
    jack->processFrames( buffers->nframes );
  }
  
  
  /*
  int tick = int( (beatFloat - beat) * 1920 );
  
  buffers->transportPosition->valid = (JackPositionBBT); | JackTransportPosition);
  
  buffers->transportPosition->tick += (buffers->nframes / buffers->samplerate);
  
  buffers->transportPosition->beat = beat % 4;
  buffers->transportPosition->tick = 0;
  
  buffers->transportPosition->ticks_per_beat = 1920;
  */
  
  int bpm = ( samplerate * 60) / fpb;
  if ( buffers->transportPosition )
  {
    //LUPPP_NOTE("BPM = %i " , bpm );
    buffers->transportPosition->valid = (JackPositionBBT);
    buffers->transportPosition->beats_per_minute = bpm;
  }
}

