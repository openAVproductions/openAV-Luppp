
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
    oldBeat(0),
    observers()
{
  samplerate = jack->getSamplerate();
  // 120 BPM default
  fpb = samplerate / 2;
  
  tapTempoPos = 0;
  tapTempo[0] = 0;
  tapTempo[1] = 0;
  tapTempo[2] = 0;
}


int TimeManager::getFpb()
{
  return fpb;
}


void TimeManager::setBpm(float b)
{
  float bpm = b * 160 + 60; // 60 - 220
#ifdef DEBUG_TIME
  LUPPP_NOTE("%s %f","setBpm()",bpm);
#endif
  setFpb( samplerate / bpm * 60 );
}


void TimeManager::setFpb(float f)
{
  fpb = f;
  //LUPPP_NOTE("%s %f","setFpb()", fpb);
  
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
  if ( tapTempo[0] < frame - samplerate * 5 )
  {
    tapTempoPos = 0;
  }
  
  if ( tapTempoPos < 3 )
  {
    tapTempo[tapTempoPos] = frame;
    tapTempoPos++;
  }
  else
  {
    // calculate frames per tap
    int tapFpb1 = tapTempo[1] - tapTempo[0];
    int tapFpb2 = tapTempo[2] - tapTempo[1];
    int tapFpb3 = frame - tapTempo[2]; // last tap, until now
    
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
  return nframesToBeat;
}

bool TimeManager::beatInThisProcess()
{
  return beatInProcess;
}


void TimeManager::process(Buffers* buffers)
{
  // tap tempo measurements
  frame = buffers->transportFrame;
  
  //int framesPerBeat = (int) buffers->samplerate / (bpm / 60.0);
  
  // time signature?
  //buffers->transportPosition->beats_per_bar = 4;
  //buffers->transportPosition->beat_type     = 4;
  
  
  // calculate beat / bar position in nframes
  int beat = buffers->transportFrame / fpb;
  nframesToBeat = buffers->transportFrame - (beat*fpb);
  
  if ( beat != oldBeat )
  {
    beatInProcess = true;
    
    if ( nframesToBeat > buffers->nframes )
    {
      nframesToBeat = buffers->nframes;
    }
    
    
    // process *upto* beat frame:
    jack->processFrames( nframesToBeat );
    
    // inform observers of new beat FIRST
    for(uint i = 0; i < observers.size(); i++)
    {
      observers.at(i)->beat();
    }
    
    // FIXME: 4 == beats in time sig
    if ( beat % 4 == 0 )
    {
      // inform observers of new bar SECOND
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->bar();
      }
      //buffers->transportPosition->bar++;
    }
    
    // process frames after beat()
    int remaining = buffers->nframes - nframesToBeat;
    if ( remaining > 0 )
    {
      /*
      char buffer [50];
      sprintf (buffer, "remaining %i",  remaining );
      EventGuiPrint e2( buffer );
      writeToGuiRingbuffer( &e2 );
      */
      jack->processFrames( remaining );
    }
    // write new beat to UI (bar info currently not used)
    EventTimeBarBeat e( 0, beat );
    writeToGuiRingbuffer( &e );
    
    oldBeat = beat;
  }
  else
  {
    beatInProcess = false;
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

