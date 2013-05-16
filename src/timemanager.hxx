
#ifndef LUPPP_TIME_H
#define LUPPP_TIME_H

#include <iostream>
#include <cstdio>

#include "buffers.hxx"
#include "eventhandler.hxx"

#include "observer/observer.hxx"

using namespace std;


// inherits from ObserverSubject
class TimeManager
{
  public:
    TimeManager():
        fpb(120),
        oldBeat(0)
    {
      tapTempoPos = 0;
      tapTempo[0] = 0;
      tapTempo[1] = 0;
      tapTempo[2] = 0;
    }
    
    void setBpm(float bpm)
    {
      setFpb( 44100 / bpm * 60 );
    }
    void setFpb(float f)
    {
      fpb = f;
      
      char buffer [50];
      sprintf (buffer, "TM, setFpb() %i", int(f) );
      EventGuiPrint e( buffer );
      writeToGuiRingbuffer( &e );
      
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->setFpb(fpb);
      }
    }
    
    void registerObserver(Observer* o)
    {
      observers.push_back(o);
      o->setFpb( fpb );
    }
    
    void tap()
    {
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
        
        int average = (tapFpb1 + tapFpb2) / 2;
        setFpb(average);
        
        char buffer [50];
        sprintf (buffer, "TM, tap() average = %i", average );
        EventGuiPrint e( buffer );
        writeToGuiRingbuffer( &e );
        
        // reset, so next 3 taps restart process
        tapTempoPos = 0;
      }
    }
    
    void process(Buffers* buffers)
    {
      // tap tempo measurements
      frame = buffers->transportFrame;
      
      //int framesPerBeat = (int) buffers->samplerate / (bpm / 60.0);
      
      
      
      // time signature?
      buffers->transportPosition->beats_per_bar = 4;
      buffers->transportPosition->beat_type     = 4;
      
      int beat = buffers->transportFrame / fpb;
      //int beat = int(beat);
      
      //int tick = int( (beatFloat - beat) * 1920 );
      
      if ( beat != oldBeat )
      {
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
        
        oldBeat = beat;
      }
      
      buffers->transportPosition->valid = JackPositionBBT;
      buffers->transportPosition->tick += (buffers->nframes / buffers->samplerate);
      
      buffers->transportPosition->beat = beat % 4;
      buffers->transportPosition->tick = 0;
      
      buffers->transportPosition->ticks_per_beat = 1920;
      
      int bpm = int(buffers->samplerate * fpb / 60.0);
      buffers->transportPosition->beats_per_minute = bpm;
    }
  
  private:
    float fpb;
    int oldBeat;
    
    // tap tempo measurements
    int frame;
    
    int tapTempoPos;
    int tapTempo[3];
    
    std::vector<Observer*> observers;
};

#endif // LUPPP_TIME_H
