
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


void TimeManager::setBpm(float bpm)
{
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


void TimeManager::process(Buffers* buffers)
{
  // tap tempo measurements
  frame = buffers->transportFrame;
  
  //int framesPerBeat = (int) buffers->samplerate / (bpm / 60.0);
  
  // time signature?
  buffers->transportPosition->beats_per_bar = 4;
  buffers->transportPosition->beat_type     = 4;
  
  
  int beat = buffers->transportFrame / fpb;
  
  // calculate beat / bar position in nframes
  int nframesRemainder = buffers->transportFrame % int(fpb);
  
  //int tick = int( (beatFloat - beat) * 1920 );
  
  if ( beat != oldBeat )
  {
    LUPPP_NOTE("Beat %i, nframes %i", beat, nframesRemainder );
    
    // inform observers of new beat FIRST
    for(uint i = 0; i < observers.size(); i++)
    {
      observers.at(i)->beat();
    }
    
    if ( beat % (int)buffers->transportPosition->beats_per_bar == 0 )
    {
      // inform observers of new bar SECOND
      for(uint i = 0; i < observers.size(); i++) { observers.at(i)->bar(); }
      buffers->transportPosition->bar++;
    }
    
    // write new beat to UI (bar info currently not used)
    EventTimeBarBeat e( 0, beat );
    writeToGuiRingbuffer( &e );
    
    oldBeat = beat;
  }
  
  
  buffers->transportPosition->valid = JackPositionBBT;
  buffers->transportPosition->tick += (buffers->nframes / buffers->samplerate);
  
  buffers->transportPosition->beat = beat % 4;
  buffers->transportPosition->tick = 0;
  
  buffers->transportPosition->ticks_per_beat = 1920;
  
  int bpm = ( samplerate * 60) / fpb;
  //LUPPP_NOTE("BPM = %i " , bpm );
  buffers->transportPosition->beats_per_minute = bpm;
  
}

