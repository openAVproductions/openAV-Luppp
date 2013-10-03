
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include <iostream>
#include <stdio.h>

#include "buffers.hxx"
#include "config.hxx"
#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

/** TrackOutput
 *  This class is used to mix down each track, and produce output: that output
 *  exists in the master outputs and send buffers.
**/
class TrackOutput : public AudioProcessor
{
  public:
    TrackOutput(int t, AudioProcessor* ap);
    
    /// set main mix, 0-1
    void setMaster(float value);
    
    float getMaster();
    
    bool recordArm();
    void recordArm(bool r);
    
    /// set send
    void setSend( int send, float value );
    void setSendActive( int send, bool active );
    
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
    
    bool _toPostfaderActive;
    bool _toKeyActive;
    bool _toXSideActive;
    
    /// Pointer to "previous" processor: the graph is backwards
    AudioProcessor* previousInChain;
    
    // Metering variables
    long uiUpdateCounter;
    long uiUpdateConstant;
    DBMeter* dbMeter;
  
};

#endif // LUPPP_TRACK_OUTPUT_H

