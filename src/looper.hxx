
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <iostream>

#include "buffers.hxx"

using namespace std;

class Looper
{
  public:
    enum State {
      STATE_PLAYING = 0,
      STATE_RECORDING,
      STATE_STOPPED,
    };
    
    Looper(int t) :
      track(t),
      state(STATE_STOPPED),
      endPoint   (0),
      playPoint  (0)
    {
      
    }
    
    void setState(State s)
    {
      // quantize?!
      state = s;
      
      if ( state == STATE_PLAYING ) // setup PLAY
      {
        endPoint = lastWrittenSampleIndex;
        playPoint = 0;
        //cout << "State = PLAYING, endPoint = " << endPoint << endl;
      }
      else if ( state == STATE_RECORDING ) // setup REC
      {
        //cout << "State = RECORDING" << endl;
        playPoint = 0;
        endPoint = 0;
        lastWrittenSampleIndex = 0;
      }
      else if ( state == STATE_STOPPED ) // 
      {
        //cout << "State = STOPPED" << endl;
      }
      
    }
    
    void process(int nframes, Buffers* buffers)
    {
      float* in  = buffers->audio[Buffers::MASTER_INPUT];
      float* out = buffers->audio[Buffers::MASTER_OUTPUT];
      
      if ( state == STATE_PLAYING )
      {
        for(int i = 0; i < nframes; i++)
        {
          if ( playPoint >= endPoint )
          {
            playPoint = 0;
          }
          out[i] += sample[playPoint++];
        }
      }
      
      else if ( state == STATE_RECORDING )
      {
        for(int i = 0; i < nframes; i++)
        {
          if ( lastWrittenSampleIndex < 44100 * 60 )
          {
            sample[lastWrittenSampleIndex++] = in[i];
          }
        }
      }
      
      // buffers pre-zeroed, so just do nothing
      else if ( state == STATE_STOPPED ){}
    }
  
  private:
    int track;
    State state;
    
    int endPoint, playPoint, lastWrittenSampleIndex;
    float sample[44100*60];
  
};

#endif // LUPPP_LOOPER_H
