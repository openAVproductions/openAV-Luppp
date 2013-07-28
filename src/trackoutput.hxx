
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
      
      for(int i = 0; i < nframes; i++)
      {
        // copy data here
        
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

