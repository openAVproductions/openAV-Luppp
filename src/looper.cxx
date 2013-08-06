

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
  tmpRecordBuffer = (float*)malloc( sizeof(float) * MAX_BUFFER_SIZE );
  memset( tmpRecordBuffer, 0, sizeof(float) * MAX_BUFFER_SIZE );
  
  for(int i = 0; i < 10; i++ )
  {
    clips[i] = LooperClip();
  }
  
  // init faust pitch shift variables
  fSamplingFreq = 44100;
  IOTA = 0;
  
  tmpBuffer.resize(MAX_BUFFER_SIZE);
  
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

void Looper::midi(unsigned char* data)
{
  /*
  if ( data[0] - 144 == track )
  {
    switch ( data[1] )
    {
      case 48: setState( STATE_RECORD_QUEUED );     break;
      case 53: case 54: case 55: case 56: case 57:
          setState( STATE_PLAY_QUEUED );
          setScene( data[1] - 53 );
          break;
      case 52: setState( STATE_STOP_QUEUED   );     break;
    }
  }
  else if ( data[0] - 128 == track )
  {
    switch ( data[1] )
    {
      case 48: setState( STATE_STOP_QUEUED );
    }
  }
  else if ( data[0] - 176 == track )
  {
    switch ( data[1] )
    {
      //case  7: gain = int(data[2])/127.f; break;
      case 7:{
          printf("volue\n");
          // volume from APC 
          EventTrackVol e( track, data[2] / 127.f );
          writeToGuiRingbuffer( &e ); }
        break;
    }
  }
  */
}

void Looper::queuePlayScene( int sc )
{
  //queuedScene = sc;
  //jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_PLAY_QUEUED);
}

//void Looper::setState( State s) {
  /*
  // quantize recording to next bar event
  if ( state == STATE_RECORDING )
  {
    stopRecordOnBar = true;
  }
  state = s;
  updateControllers();
}  */


void Looper::updateControllers()
{
  /*
  if (state == STATE_RECORD_QUEUED )
  {
    numBeats = 0;
    jack->getControllerUpdater()->recordArm(track, true);
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_RECORD_QUEUED);
  }
  else if (state == STATE_RECORDING )
  {
    jack->getControllerUpdater()->recordArm(track, true);
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_RECORDING);
  }
  else
  {
    jack->getControllerUpdater()->recordArm(track, false);
  }
  
  if (state == STATE_PLAY_QUEUED )
  {
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_PLAY_QUEUED);
  }
  
  if ( state == STATE_PLAYING )
  {
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_PLAYING);
  }
  
  if (state == STATE_STOP_QUEUED )
  {
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_STOP_QUEUED);
  }
  else if ( state == STATE_STOPPED )
  {
    jack->getControllerUpdater()->clipSelect(track, scene, Controller::CLIP_MODE_LOADED);
    EventLooperProgress e(track, 0 );
    writeToGuiRingbuffer( &e );
  }
  */
}

void Looper::setSample(int scene, AudioBuffer* ab)
{
  clips[scene].load( ab );
  /*
  vector<float>& buf = ab->getData();
  if ( buf.size() > SAMPLE_SIZE )
  {
    EventGuiPrint e( "Looper setSample() ERROR size > incoming sample" );
    writeToGuiRingbuffer( &e );
  }
  else
  {
    char buffer [50];
    sprintf (buffer, "Looper setSample() writing to scene %i",scene);
    EventGuiPrint e( buffer );
    writeToGuiRingbuffer( &e );
    
    numBeats = ab->getBeats();
    float* s = &sample[sc];
    float* b = &buf[0];
    for (unsigned int i = 0; i < buf.size(); i++)
    {
      *s++ = *b++;
    }
    
    endPoint = buf.size();
    lastWrittenSampleIndex = buf.size();
    
    //memcpy( &sample[0], &buf[0], buf.size() ); // copy sample data to pre-allocated buffer
  }
  */
}

void Looper::process(int nframes, Buffers* buffers)
{
  float* in  = buffers->audio[Buffers::MASTER_INPUT];
  
  // FIXME:
  // using the track output causes distortion: clipping / not proper writing.
  // writing to master fixes issue, so its due to trackOutput or Looper writing...?
  float* out = buffers->audio[Buffers::TRACK_0 + track];
  //float* out = buffers->audio[Buffers::MASTER_OUTPUT];
  
  // process each clip individually: this allows for playback of one clip,
  // while another clip records.
  for ( int clip = 0; clip < NSCENES; clip++ )
  {
    // handle state of clip, and do what needs doing:
    // record into buffer, play from buffer, etc
    if ( clips[clip].recording() )
    {
      // copy data from input buffer to recording buffer
      
      if ( clips[clip].recordSpaceAvailable() <  LOOPER_SAMPLES_BEFORE_REQUEST )
      {
        EventLooperClipRequestBuffer e( track, clip, clips[clip].audioBufferSize() + 44100 );
        writeToGuiRingbuffer( &e );
      }
    }
    else if ( clips[clip].playing() )
    {
      //printf("Looper %i playing()\n", track );
      // copy data into tmpBuffer, then pitch-stretch into track buffer
      for(int i = 0; i < nframes; i++ )
      {
        out[i] = clips[clip].getSample();
      }
      
      // update UI of progress
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
      
      //*out++ = sin( playPoint * 440 * 6.24 );
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
  /*
  // stopRecordOnBar ensures we record right up to the bar measure
  else if ( state == STATE_RECORDING || stopRecordOnBar )
  {
    for(int i = 0; i < nframes; i++)
    {
      if ( lastWrittenSampleIndex < SAMPLE_SIZE )
      {
        sample[lastWrittenSampleIndex++] = in[i];
      }
    }
  }
  */
}


void Looper::bar()
{
  /*
  int barTmpState = state;
  // queue stop recording -> stop recording, now calculate beats in loop
  if ( stopRecordOnBar )
  {
    stopRecordOnBar = false;
  }
  
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
    
    EventLooperState e2( track, scene, STATE_PLAYING );
    writeToGuiRingbuffer( &e2 );
  }
  if ( state == STATE_RECORD_QUEUED )
  {
    EventGuiPrint e( "Looper Q->Recording" );
    writeToGuiRingbuffer( &e );
    
    EventLooperState e2( track, scene, STATE_RECORDING );
    writeToGuiRingbuffer( &e2 );
    
    state = STATE_RECORDING;
    playPoint = 0;
    endPoint = 0;
    lastWrittenSampleIndex = 0;
  }
  if ( state == STATE_STOP_QUEUED )
  {
    EventGuiPrint e( "Looper Q->Stopped" );
    writeToGuiRingbuffer( &e );
    
    EventLooperState e2( track, scene, STATE_STOPPED );
    writeToGuiRingbuffer( &e2 );
    
    state = STATE_STOPPED;
    endPoint = lastWrittenSampleIndex;
  }
  
  if ( barTmpState != state )
  {
    updateControllers();
  }
  */
}

void Looper::beat()
{
  /*
  if (state == STATE_RECORDING || stopRecordOnBar )
  {
    numBeats++;
  }
  playedBeats++;
  * 
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
