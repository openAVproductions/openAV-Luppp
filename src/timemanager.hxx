
#ifndef LUPPP_TIME_H
#define LUPPP_TIME_H

#include <iostream>

#include "buffers.hxx"

using namespace std;

// inherits from ObserverSubject
class TimeManager
{
  public:
    TimeManager()
    {
    }
    
    void process(Buffers* buffers)
    {
      float bpm = 120;
      int framesPerBeat = (int) samplerate / (bpm / 60.0);
      
      // divide by zero errors!
      if ( framesPerBeat > 0 )
      {
        int newBeat = buffers->transportFrame / framesPerBeat;
      }
      //cout << buffers->transportPosition.frame << endl;
    }
  
  private:
    int samplerate;
    
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
