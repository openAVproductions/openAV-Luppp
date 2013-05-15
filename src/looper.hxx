
#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <iostream>

#include "buffers.hxx"

#include "observer/observer.hxx"

using namespace std;

class Looper : public Observer // for notifications
{
  public:
    enum State {
      STATE_PLAYING         = 0x01,
      STATE_PLAY_QUEUED     = 0x02,
      STATE_RECORDING       = 0x03,
      STATE_RECORD_QUEUED   = 0x04,
      STATE_STOPPED         = 0x05,
      STATE_STOP_QUEUED     = 0x06,
    };
    
    Looper(int t) :
      track(t),
      state(STATE_STOPPED),
      endPoint   (0),
      playPoint  (0),
      lastWrittenSampleIndex(0)
    {
      
    }
    
    void bar()
    {
      cout << "Looper " << track << " got bar()" << flush;
      playPoint = 0;
      
      if ( state == STATE_PLAY_QUEUED )
      {
        cout << "  Q->Playing  endpoint = " << endPoint;
        state = STATE_PLAYING;
        playPoint = 0;
        endPoint = lastWrittenSampleIndex;
      }
      if ( state == STATE_RECORD_QUEUED )
      {
        cout << "  Q->Recording ";
        state = STATE_RECORDING;
        playPoint = 0;
        endPoint = 0;
        lastWrittenSampleIndex = 0;
      }
      if ( state == STATE_PLAY_QUEUED )
      {
        cout << "  Q->Stopped ";
        state = STATE_STOPPED;
        endPoint = lastWrittenSampleIndex;
      }
      cout << endl;
    }
    
    void beat()
    {
      //cout << "Looper " << track << " got beat()" << flush;
    }
    
    void setFpb(int f)
    {
      fpb = f;
      //cout << "Looper " << track << " got fpb of " << fpb << endl;
    }
    
    void setState(State s);
    
    void process(int nframes, Buffers* buffers)
    {
      float* in  = buffers->audio[Buffers::MASTER_INPUT];
      float* out = buffers->audio[Buffers::MASTER_OUTPUT];
      
      if ( state == STATE_PLAYING )
      {
        for(int i = 0; i < nframes; i++)
        {
          if ( playPoint < endPoint )
          {
            out[i] += sample[playPoint++];
          }
          
        }
      }
      
      else if ( state == STATE_RECORDING )
      {
        cout << "recording " << endl;
        for(int i = 0; i < nframes; i++)
        {
          if ( lastWrittenSampleIndex < 44100 * 60 )
          {
            sample[lastWrittenSampleIndex++] = in[i];
          }
        }
      }
    }
  
  private:
    int track;
    State state;
    
    int fpb;
    
    int endPoint, playPoint, lastWrittenSampleIndex;
    float sample[44100*60];
  
};

#endif // LUPPP_LOOPER_H
