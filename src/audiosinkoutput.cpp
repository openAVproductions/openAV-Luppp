
#include "audiosinkoutput.hpp"

int AudioSinkOutput::privateID = 0;

AudioSinkOutput::AudioSinkOutput(Top* t)
{
  ID = privateID++;
  
  std::cout << "AudioSinkOutput() ID = " << ID << std::endl;
  
  top = t;
}

void AudioSinkOutput::process(int nframes, float* in, float *W, float *X, float *Y, float* Z)
{
  TrackOutputState* state = top->state.getAudioSinkOutput(ID);
  
  //std::cout << "AudioSinkOutput::process() vol = " << state->volume << " pan = " << state->pan << std::endl;
  
  float logVolume = state->volume;
  float pan = -1 * state->pan; // invert pan, way the algorithm works!
  float panZ = 0.f;
  
  // azimuth
  float cosAzimuth  = cos( pan * 3.14 );
  float sinAzimuth  = sin( pan * 3.14 );
  
  // elevation
  float cosElevation  = cos( panZ );
  float sinElevation  = sin( panZ );
  
  /*
  ; calculate cos and sin of 
  ; time-varying angles 
  kca = cos(kone)  ; azimuth
  ksa = sin(kone) 
  kcb = cos(ktwo)  ; elevation
  ksb = sin(ktwo)

  ; B-format encoding equations

  ax = a5 * kca * kcb 
  ay = a5 * ksa * kcb 
  az = a5 * ksb 
  aw = a5 * 0.707
  */
  
  float tmp;
  
  for( int i = 0; i < nframes; i++)
  {
    // += so we don't overwrite the previous tracks!
    tmp = in[i] * 0.707 * logVolume;
    W[i] += tmp;
    X[i] += in[i] * cosAzimuth * cosElevation * logVolume;
    Y[i] += in[i] * sinAzimuth * cosElevation * logVolume;
    Z[i] += in[i] * sinElevation * logVolume;
    
    // clear the internal track buffer
    in[i] = 0.f;
    
    if ( maxAmplitude < tmp )
      maxAmplitude = tmp;
  }
  
  amplitudeSendCounter -= nframes;
  
  std::cout << "SinkOutput " << ID << " processing now" << std::endl;
  
  if ( amplitudeSendCounter < 0 )
  {
    std::cout << "sending track RMS now on track " << ID << std::endl;
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setTrackRms( ID, maxAmplitude );
    top->toGuiQueue.push(x);
    
    maxAmplitude = 0.f;
    amplitudeSendCounter = top->samplerate / 10;
  }
}
