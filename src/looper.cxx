

#include "looper.hxx"

#include "jack.hxx"
#include "eventhandler.hxx"

extern Jack* jack;

void Looper::setState(State s)
{
  // ensure we're not setting eg PLAY_QUEUED, if we're already PLAYING
  if ( static_cast<int>(s) != static_cast<int>(state) + 1)
  {
    cout << "new state " << s << endl;
    state = s;
  }
}

void Looper::process(int nframes, Buffers* buffers)
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
    
    // update UI
    EventLooperProgress e(track, float(playPoint) / endPoint );
    writeToGuiRingbuffer( &e );
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
