
#include "time.hpp"

#include "top.hpp"

using namespace std;

Time::Time(Top* t)
{
  top = t;
}

// this function gets called by JACK before processing any audio for
// nframes, allowing it to modify Engine's state before it's run.
void Time::process(int frameNumber)
{
  float bpm = top->bpm;
  
  int framesPerBeat = (int) top->samplerate / (bpm / 60.0);
  
  int newBeat = frameNumber / framesPerBeat;
  
  // here we handle *all* events that should occur on *a* beat
  if ( newBeat != beat )
  {
    cout << "Time:P() bpm = " << bpm << "  FPB: " << framesPerBeat << "  beat: " << newBeat << endl;
    beat = newBeat;
    
    
    
    if ( newBeat % 16 == 0 )
    {
      cout << "Time:P() 16th beat!" << endl;
    }
    
    if ( newBeat % 4 == 0 )
    {
      cout << "Time:P() 4th beat!" << endl;
    }
    
  }
  
}
