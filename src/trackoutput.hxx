
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include <iostream>
#include <stdio.h>

#include "buffers.hxx"
#include "config.hxx"
#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

class TrackOutput : public AudioProcessor
{
  public:
    TrackOutput(int t, AudioProcessor* ap);
    
    /// set main mix, 0-1
    void setMaster(float value);
    
    float getMaster();
    
    bool recordArm()
    {
      return _recordArm;
    }
    void recordArm(bool r)
    {
      _recordArm = r;
    }
    
    /// set send
    void setSend( int send, float value );
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    void process(unsigned int nframes, Buffers* buffers);
    
    ~TrackOutput();
  
  private:
    int track;
    
    bool  _recordArm;
    
    float _toMaster;
    
    float _toReverb;
    float _toSidechain;
    float _toPostSidechain;
    
    /// Pointer to "previous" processor: the graph is backwards
    AudioProcessor* previousInChain;
    
    // Metering variables
    long uiUpdateCounter;
    long uiUpdateConstant;
    DBMeter* dbMeter;
  
};

#endif // LUPPP_TRACK_OUTPUT_H

