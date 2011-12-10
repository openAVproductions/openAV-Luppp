
#include "audiosinkoutput.hpp"

int AudioSinkOutput::privateID = 0;

AudioSinkOutput::AudioSinkOutput(Top* t)
{
  ID = privateID++;
  
  top = t;
  
  volume = 0.f;
  pan    = 0.f;
  panZ   = 0.f;
}

void AudioSinkOutput::setParameter(int, float)
{
  
}

void AudioSinkOutput::setPan(float inPan)
{
  if (inPan > 1.0)
    inPan = 1.0;
  
  if (inPan < -1.0)
    inPan = -1.0;
  
  pan = inPan;
  
  std::cout << "AudioSinkOutput() pan = " << pan << std::endl;
}

void AudioSinkOutput::setVolume(float vol)
{
  // Clip vol input to range
  if (vol > 1.0)
    vol = 1.0;
  else if (vol < 0.00001)
    vol = 0.0;
  
  // Keep a copy for "input" [0,1] volume (linear)
  volume = vol;
  
  // linear input, hence we need to convert the [0,1] range
  // to a logarithmic scale. Formula used:
  // y = x ^ 3, this is an approximation of the human hearing curve.
  // http://www.dr-lex.be/info-stuff/logVolumecontrols.html for info
  
  float volMultiplier = (float)pow( vol , 3);
  
  // apply scaling to volMultiplier, so we can amplify to +50%
  logVolume = volMultiplier * 2;
  
  std::cout << "Volume::setVolume(), inVol:" << vol << "  final:" << logVolume << std::endl;
}

void AudioSinkOutput::process(int nframes, float* in, float *W, float *X, float *Y, float* Z)
{
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
  
  for( int i = 0; i < nframes; i++)
  {
    // += so we don't overwrite the previous tracks!
    W[i] += in[i] * 0.707 * logVolume;
    X[i] += in[i] * cosAzimuth * cosElevation * logVolume;
    Y[i] += in[i] * sinAzimuth * cosElevation * logVolume;
    Z[i] += in[i] * sinElevation * logVolume;
  }
}
