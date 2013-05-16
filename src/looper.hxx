
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
      STATE_PLAYING = 0,
      STATE_PLAY_QUEUED,
      STATE_RECORDING,
      STATE_RECORD_QUEUED,
      STATE_STOPPED,
      STATE_STOP_QUEUED,
    };
    
    Looper(int t) :
      track(t),
      state(STATE_STOPPED),
      numBeats   (4),
      playedBeats(0),
      endPoint   (0),
      playPoint  (0),
      lastWrittenSampleIndex(0)
    {
      
    }
    
    void bar()
    {
      // only reset if we're on the last beat of a loop
      if ( playedBeats >= numBeats + 1 )
      {
        //cout << "Looper " << track << " restting to 0 " << endl;
        playPoint = 0;
        playedBeats = 0;
      }
      
      if ( state == STATE_PLAY_QUEUED )
      {
        cout << "  Q->Playing  endpoint = " << endPoint << endl;
        state = STATE_PLAYING;
        playPoint = 0;
        endPoint = lastWrittenSampleIndex;
      }
      if ( state == STATE_RECORD_QUEUED && state != STATE_RECORDING )
      {
        cout << "  Q->Recording " << endl;
        state = STATE_RECORDING;
        playPoint = 0;
        endPoint = 0;
        lastWrittenSampleIndex = 0;
      }
      if ( state == STATE_PLAY_QUEUED && state != STATE_STOPPED )
      {
        cout << "  Q->Stopped " << endl;
        state = STATE_STOPPED;
        endPoint = lastWrittenSampleIndex;
      }
    }
    
    void setLoopLength(float l)
    {
      numBeats *= l;
      
      // avoid the 0 * 2 problem
      if ( numBeats < 1 )
        numBeats = 1;
      
      cout << "Looper " << track << " loop lenght now " << numBeats << endl;
    }
    
    void beat()
    {
      playedBeats++;
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
    int numBeats;
    int playedBeats;
    
    int endPoint, playPoint, lastWrittenSampleIndex;
    float sample[44100*60];
  
};

#endif // LUPPP_LOOPER_H
