
#ifndef LUPPP_TIME_H
#define LUPPP_TIME_H

#include <iostream>

#include "buffers.hxx"

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
      bpm = b;
      for(uint i = 0; i < observers.size(); i++)
      {
        observers.at(i)->setFpb(bpm);
      }
    }
    
    void registerObserver(Observer* o)
    {
      observers.push_back(o);
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
        if ( beat % (int)buffers->transportPosition->beats_per_bar == 0 )
        {
          // inform observers of new bar
          for(uint i = 0; i < observers.size(); i++) { observers.at(i)->bar(); }
          buffers->transportPosition->bar++;
        }
        
        // inform observers of new beat
        for(uint i = 0; i < observers.size(); i++)
        {
          observers.at(i)->beat();
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
    
    // list of Observers of this TimeManager Subject, "beat", "bar" updates?
    /*
    for(int i = 0; i < numObservers; i++ )
    {
      observers[i]->notifyNewBeat(int beat);
    }
    
    if ( bar != oldBar )
    {
      for(int i = 0; i < numObservers; i++ )
      {
        observers[i]->notifyNewBar(int bar);
      }
    }
    */
    
};

#endif // LUPPP_TIME_H
