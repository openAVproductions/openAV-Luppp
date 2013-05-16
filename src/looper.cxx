

#include "looper.hxx"

#include "jack.hxx"
#include "eventhandler.hxx"

extern Jack* jack;

void Looper::setState(State s)
{
  if ( state == STATE_RECORDING )
  {
    stopRecordOnBar = true;
  }
  
  // ensure we're not setting eg PLAY_QUEUED, if we're already PLAYING
  if ( static_cast<int>(s) != static_cast<int>(state) + 1)
  {
    state = s;
    
    if (state == STATE_RECORDING)
    {
      numBeats = 0;
    }
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
        out[i] += sample[playPoint];
      }
      // always update playPoint, even when not playing sound.
      // it updates the UI of progress
      playPoint++;
    }
    
    float prog = (float(playPoint) / (fpb*numBeats));
    
    /*
    if ( track == 0 )
      cout << prog << "  playPoint " << playPoint << "  endPoint " << endPoint
           << "  fpb*numBeats " << fpb * numBeats <<  endl;
    */
    EventLooperProgress e(track, prog );
    writeToGuiRingbuffer( &e );
  }
  
  // stopRecordOnBar ensures we record right up to the bar measure
  else if ( state == STATE_RECORDING || stopRecordOnBar )
  {
    for(int i = 0; i < nframes; i++)
    {
      if ( lastWrittenSampleIndex < 44100 * 60 )
      {
        sample[lastWrittenSampleIndex++] = in[i];
      }
    }
  }
}


void Looper::bar()
{
  stopRecordOnBar = false;
  
  if ( playedBeats >= numBeats )
  {
    playPoint = 0;
    playedBeats = 0;
  }
  
  if ( state == STATE_PLAY_QUEUED )
  {
    EventGuiPrint e( "Looper Q->Playing" );
    writeToGuiRingbuffer( &e );
    state = STATE_PLAYING;
    playPoint = 0;
    endPoint = lastWrittenSampleIndex;
  }
  if ( state == STATE_RECORD_QUEUED && state != STATE_RECORDING )
  {
    EventGuiPrint e( "Looper Q->Recording" );
    writeToGuiRingbuffer( &e );
    
    state = STATE_RECORDING;
    playPoint = 0;
    endPoint = 0;
    lastWrittenSampleIndex = 0;
  }
  if ( state == STATE_PLAY_QUEUED && state != STATE_STOPPED )
  {
    EventGuiPrint e( "Looper Q->Stopped" );
    writeToGuiRingbuffer( &e );
    
    state = STATE_STOPPED;
    endPoint = lastWrittenSampleIndex;
  }
}

void Looper::beat()
{
  if (state == STATE_RECORDING)
  {
    numBeats++;
  }
  playedBeats++;   // only reset if we're on the last beat of a loop
}

void Looper::setLoopLength(float l)
{
  numBeats *= l;
  
  // smallest loop = 4 beats
  if ( numBeats < 4 )
    numBeats = 4;
  
  char buffer [50];
  sprintf (buffer, "Looper loop lenght = %i", numBeats );
  EventGuiPrint e( buffer );
  writeToGuiRingbuffer( &e );
}
