
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include <iostream>
#include <stdio.h>

#include "config.hxx"
#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

class TrackOutput : public AudioProcessor
{
  public:
    TrackOutput(int t, AudioProcessor* ap) :
      AudioProcessor(),
      track(t),
      _trackBuffer( MAX_BUFFER_SIZE, 0.f ),
      previousInChain(ap),
      dbMeter(44100)
    {
      printf("trackOutput ID: %i, ap = ", track );
      std::cout << ap << std::endl;
      
      
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
    
    /// set send
    void setSend( int send, float value )
    {
      switch( send )
      {
        case SEND_REV:
            _toReverb = value;
            break;
        case SEND_SIDE:
            _toSidechain = value;
            break;
        case SEND_POST:
            _toPostSidechain = value;
            break;
      }
      
    }
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    void process(int nframes, Buffers* buffers)
    {
      // zero track buffer
      memset( &_trackBuffer[0], 0, nframes );
      
      buffers->audio[Buffers::TRACK_0 + track] = &_trackBuffer[0];
      previousInChain->process( nframes, buffers );
      
      
      // run the meter
      float* buf = &_trackBuffer[0];
      dbMeter.process( nframes, buf, buf );
      
      if (uiUpdateCounter > uiUpdateConstant )
      {
        // FIXME: should be using ControllerUpdater
        EventTrackSignalLevel e( track, dbMeter.getLeftDB() * _toMaster, dbMeter.getRightDB() * _toMaster );
        writeToGuiRingbuffer( &e );
        uiUpdateCounter = 0;
      }
      uiUpdateCounter += nframes;
      
      
      /// copy audio data into reverb / sidechain / master buffers
      float* reverb        = buffers->audio[Buffers::REVERB];
      float* sidechain     = buffers->audio[Buffers::SIDECHAIN];
      float* postSidechain = buffers->audio[Buffers::POST_SIDECHAIN];
      
      float* masterL       = buffers->audio[Buffers::MASTER_OUT_L];
      float* masterR       = buffers->audio[Buffers::MASTER_OUT_R];
      
      for(int i = 0; i < nframes; i++)
      {
        float tmp = _trackBuffer[i];
        
        masterL[i]       += tmp * _toMaster;
        masterR[i]       += tmp * _toMaster;
        
        masterL++;
        masterR++;
        
        *reverb++        += tmp * _toReverb;
        *sidechain++     += tmp * _toSidechain;
        *postSidechain++ += tmp * _toPostSidechain;
      }
      
    }
    
    ~TrackOutput()
    {
    }
  
  private:
    int track;
    
    std::vector<float> _trackBuffer;
    
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

