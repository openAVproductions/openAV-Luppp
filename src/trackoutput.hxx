
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include <stdio.h>

#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

class TrackOutput : public AudioProcessor
{
  public:
    TrackOutput(int t, AudioProcessor* ap) :
      AudioProcessor(),
      track(t),
      previousInChain(ap),
      dbMeter(44100)
    {
      printf("trackOutput ID: %i\n", track);
      
      // UI update
      uiUpdateConstant = 44100 / 30;
      uiUpdateCounter  = 44100 / 30;
      
      _toReverb        = 0.0;
      _toMaster        = 0.8;
      _toSidechain     = 0.0;
      _toPostSidechain = 0.0;
    }
    
    /// set main mix, 0-1
    void setMaster(float value)
    {
      _toMaster = value;
    }
    /// set sidechain mix, 0-1
    void setSidechain(float value)
    {
      _toSidechain = value;
    }
    /// set post sidechain mix, 0-1
    void setPostSidechain(float value)
    {
      _toPostSidechain = value;
    }
    /// set reverb mix, 0-1
    void setReverb(float value)
    {
      _toReverb = value;
    }
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    void process(int nframes, Buffers* buffers)
    {
      // zero track buffer
      float* buf = _trackBuffer;
      for(int i = 0; i < nframes; i++ )
      {
        *buf++ = 0.f;
      }
      
      if ( previousInChain )
      {
        buffers->audio[Buffers::TRACK_0 + track] = _trackBuffer;
        //memset( _trackBuffer, 0, nframes );
        previousInChain->process( nframes, buffers );
      }
      
      // run the meter
      buf = _trackBuffer;
      dbMeter.process( nframes, buf, buf );
      
      if (uiUpdateCounter > uiUpdateConstant )
      {
        EventTrackSignalLevel e( track, dbMeter.getLeftDB() * _toMaster, dbMeter.getRightDB() * _toMaster );
        writeToGuiRingbuffer( &e );
        uiUpdateCounter = 0;
      }
      uiUpdateCounter += nframes;
      
      /// copy audio data into reverb / sidechain / master buffers
      float* trackBuf      = buffers->audio[Buffers::TRACK_0 + track];
      float* reverb        = buffers->audio[Buffers::REVERB];
      float* sidechain     = buffers->audio[Buffers::SIDECHAIN];
      float* postSidechain = buffers->audio[Buffers::POST_SIDECHAIN];
      
      float* master        = buffers->audio[Buffers::MASTER_OUTPUT];
      
      for(int i = 0; i < nframes; i++)
      {
        *reverb++        += *trackBuf * _toReverb;
        //*sidechain++     += *trackBuf * _toSidechain;
        //*postSidechain++ += *trackBuf * _toPostSidechain;
        
        *master++        += *trackBuf * _toMaster;
        
        trackBuf++;
      }
      
    }
    
    ~TrackOutput()
    {
      delete _trackBuffer;
    }
  
  private:
    int track;
    
    float _trackBuffer[1024];
    
    float _toMaster;
    
    float _toReverb;
    float _toSidechain;
    float _toPostSidechain;
    
    /// Pointer to "previous" processor: the graph is backwards
    AudioProcessor* previousInChain;
    
    // Metering variables
    long uiUpdateCounter;
    long uiUpdateConstant;
    DBMeter dbMeter;
  
};

#endif // LUPPP_TRACK_OUTPUT_H

