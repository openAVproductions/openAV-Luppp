

#include "looper.hxx"

#include "config.hxx"

#include "jack.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "controllerupdater.hxx"

extern Jack* jack;

Looper::Looper(int t) :
  AudioProcessor(),
  TimeObserver(),
  track(t),
  uiUpdateConstant(44100/30.f),
  uiUpdateCounter(44100/30.f)
{
  // pre-zero the internal sample
  //tmpRecordBuffer = (float*)malloc( sizeof(float) * MAX_BUFFER_SIZE );
  //memset( tmpRecordBuffer, 0, sizeof(float) * MAX_BUFFER_SIZE );
  
  for(int i = 0; i < 10; i++ )
  {
    clips[i] = LooperClip();
  }
  
  fpb = 22050;
  
  // init faust pitch shift variables
  fSamplingFreq = 44100;
  IOTA = 0;
  
  //tmpRecordBuffer.resize(MAX_BUFFER_SIZE);
  
  for (int i=0; i<65536; i++)
    fVec0[i] = 0;
  for (int i=0; i<2; i++)
    fRec0[i] = 0;
  semitoneShift = 0.0f;
  windowSize = 1000;
  crossfadeSize = 1000;
}

LooperClip* Looper::getClip(int scene)
{
  return &clips[scene];
}

void Looper::setRequestedBuffer(int s, AudioBuffer* ab)
{
  clips[s].setRequestedBuffer( ab );
}


void Looper::setSample(int scene, AudioBuffer* ab)
{
  clips[scene].load( ab );
  
  char buffer [50];
  sprintf (buffer, "Looper setSample() writing to scene %i",scene);
  EventGuiPrint e( buffer );
  writeToGuiRingbuffer( &e );
}

void Looper::process(unsigned int nframes, Buffers* buffers)
{
  // process each clip individually: this allows for playback of one clip,
  // while another clip records.
  for ( int clip = 0; clip < NSCENES; clip++ )
  {
    // handle state of clip, and do what needs doing:
    // record into buffer, play from buffer, etc
    if ( clips[clip].recording() )
    {
      if ( clips[clip].recordSpaceAvailable() <  LOOPER_SAMPLES_BEFORE_REQUEST &&
          !clips[clip].newBufferInTransit() )
      {
        EventLooperClipRequestBuffer e( track, clip, clips[clip].audioBufferSize() + 44100 * 4);
        writeToGuiRingbuffer( &e );
        clips[clip].newBufferInTransit(true);
      }
      
      // copy data from input buffer to recording buffer
      float* input = buffers->audio[Buffers::MASTER_INPUT];
      clips[clip].record( nframes, input, 0 );
    }
    else if ( clips[clip].playing() )
    {
      // copy data into tmpBuffer, then pitch-stretch into track buffer
      long targetFrames = clips[clip].getBeats() * fpb;
      long actualFrames = clips[clip].getBufferLenght();
      float playSpeed = 1.0;
      
      if ( targetFrames != 0 && actualFrames != 0 )
      {
        playSpeed = float(actualFrames) / targetFrames;
      }
      
      float* out = buffers->audio[Buffers::TRACK_0 + track];
      
      for(unsigned int i = 0; i < nframes; i++ )
      {
        out[i] = clips[clip].getSample(playSpeed);
      }
      
      //printf("Looper %i playing(), speed = %f\n", track, playSpeed );
      
      if ( uiUpdateCounter > uiUpdateConstant )
      {
        
        jack->getControllerUpdater()->setTrackSceneProgress(track, clip, clips[clip].getProgress() );
        uiUpdateCounter = 0;
      }
      uiUpdateCounter += nframes;
    }
  }
  
  
  /*
  float playbackSpeed = endPoint / ( float(numBeats) * fpb );
  semitoneShift = -( 12 * log ( playbackSpeed ) / log (2) );
  
  if ( state == STATE_PLAYING ||
       state == STATE_STOP_QUEUED )
  {
    for(int i = 0; i < nframes; i++)
    {
      if ( playPoint < endPoint )
      {
        tmpBuffer[i] = sample[int(playPoint)] * gain;
      }
      playPoint += 1.0; //playbackSpeed;
      
      //out++ = sin( playPoint * 440 * 6.24 );
      *trk = tmpBuffer[i];
      *out++ = *trk++;
    }
    
    // now pitch-shift the audio in the buffer
    //pitchShift( nframes, &tmpBuffer[0], out);
    
    if ( uiUpdateCounter > uiUpdateConstant )
    {
      float prog = (float(playPoint) / (fpb*numBeats));
      EventLooperProgress e(track, prog );
      writeToGuiRingbuffer( &e );
    }
    uiUpdateCounter += nframes;
  }
  */
}

void Looper::pitchShift(int count, float* input, float* output)
{
  float   fSlow0 = windowSize;
  float   fSlow1 = ((1 + fSlow0) - powf(2,(0.08333333333333333f * semitoneShift)));
  float   fSlow2 = (1.0f / crossfadeSize);
  float   fSlow3 = (fSlow0 - 1);
  float* input0 = &input[0];
  //float* output0 = &output[0];
  
  for (int i=0; i<count; i++)
  {
    float fTemp0 = (float)input0[i];
    fVec0[IOTA&65535] = fTemp0;
    fRec0[0] = fmodf((fRec0[1] + fSlow1),fSlow0);
    int iTemp1 = int(fRec0[0]);
    int iTemp2 = (1 + iTemp1);
    float fTemp3 = min((fSlow2 * fRec0[0]), 1.f );
    float fTemp4 = (fSlow0 + fRec0[0]);
    int iTemp5 = int(fTemp4);
    
    *output++ += (float)(((1 - fTemp3) * (((fTemp4 - iTemp5) * 
    fVec0[(IOTA-int((int((1 + iTemp5)) & 65535)))&65535]) + ((0 - ((
    fRec0[0] + fSlow3) - iTemp5)) * fVec0[(IOTA-int((iTemp5 & 65535)))
    &65535]))) + (fTemp3 * (((fRec0[0] - iTemp1) * fVec0[(IOTA-int((int(
    iTemp2) & 65535)))&65535]) + ((iTemp2 - fRec0[0]) * fVec0[(IOTA-int((
    iTemp1 & 65535)))&65535]))));
    
    fRec0[1] = fRec0[0];
    IOTA = IOTA+1;
  }
}
