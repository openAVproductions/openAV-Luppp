
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
        bpm(120),
        oldBeat(0)
    {
    }
    
    void setBpm(int b)
    {
      char buffer [50];
      sprintf (buffer, "%d", b);
      //printf ("[%s] is a string %d chars long\n",buffer,n);
      
      EventGuiPrint e( buffer );
      writeToGuiRingbuffer( &e );
      
      bpm = b;
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->setFpb(bpm);
      }
    }
    
    void registerObserver(Observer* o)
    {
      observers.push_back(o);
      int fpb = 44100 / bpm * 60;
      char buffer [50];
      sprintf (buffer, "TM, bpm = %i, fpb = %i", int(bpm), fpb );
      EventGuiPrint e( buffer );
      writeToGuiRingbuffer( &e );
      
      // triggers newly registered object to have bpm set
      o->setFpb( fpb );
    }
    
    void process(Buffers* buffers)
    {
      int framesPerBeat = (int) buffers->samplerate / (bpm / 60.0);
      
      // time signature?
      buffers->transportPosition->beats_per_bar = 4;
      buffers->transportPosition->beat_type     = 4;
      
      int beat = buffers->transportFrame / framesPerBeat;
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
      buffers->transportPosition->beats_per_minute = bpm;
    }
  
  private:
    float bpm;
    int oldBeat;
    
    std::vector<Observer*> observers;
};

#endif // LUPPP_TIME_H
