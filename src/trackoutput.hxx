
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

class TrackOutput : public AudioProcessor
{
  public:
    TrackOutput(int t, AudioProcessor* ap) :
      track(t),
      previousInChain(ap),
      dbMeter(44100)
    {
      // UI update
      uiUpdateConstant = 44100 / 20;
      uiUpdateCounter  = 44100 / 30;
    }
    
    /// set main mix, 0-1
    void setMaster(float value)
    {
      
    }
    /// set sidechain mix, 0-1
    void setSidechain(float value)
    {
      
    }
    /// set post sidechain mix, 0-1
    void setPostSidechain(float value)
    {
      
    }
    /// set reverb mix, 0-1
    void setReverb(float value)
    {
      
    }
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    void process(int nframes, Buffers* buffers)
    {
      if ( previousInChain )
      {
        previousInChain->process( nframes, buffers );
      }
      
      float* buf = buffers->audio[Buffers::TRACK_0 + track];
      dbMeter.process( nframes, buf, buf );
      
      if (uiUpdateCounter > uiUpdateConstant )
      {
        EventTrackSignalLevel e( track, dbMeter.getLeftDB(), dbMeter.getRightDB() );
        writeToGuiRingbuffer( &e );
        uiUpdateCounter = 0;
      }
      uiUpdateCounter += nframes;
      
      
      /// copy audio data into reverb / sidechain / master buffers
      float* trackBuf      = buffers->audio[Buffers::TRACK_0 + track];
      float* reverb        = buffers->audio[Buffers::REVERB];
      float* sidechain     = buffers->audio[Buffers::SIDECHAIN];
      float* postSidechain = buffers->audio[Buffers::POST_SIDECHAIN];
      
      for(int i = 0; i < nframes; i++)
      {
        *reverb++        += *trackBuf * _toReverb;
        *sidechain++     += *trackBuf * _toSidechain;
        *postSidechain++ += *trackBuf * _toPostSidechain;
        
        trackBuf++;
      }
    }
  
  private:
    int track;
    
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

