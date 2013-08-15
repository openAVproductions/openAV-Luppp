
#ifndef OPENAV_DSP_SIDECHAIN_GAIN_H
#define OPENAV_DSP_SIDECHAIN_GAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class SidechainGain
{
  public:
    SidechainGain(int rate);
    ~SidechainGain(){}
    
    void threshold(float t){controlThreshold = t;}
    void reduction(float r){controlReduction = r;}
    void time     (float t){controlReleaseTime = t;}
    
    void process(unsigned int n_samples, float** inputs, float** outputs);
  
  private:
    /// audio buffers
    float* audioInputL;
    float* audioInputR;
    float* audioSidechain;
    float* audioOutputL;
    float* audioOutputR;
    
    /// control signals
    float controlThreshold;
    float controlReduction;
    float controlReleaseTime;
    float controlSidechainAmp;
    
    /// filter state
    float w, a, b, g1, g2;
    
    /// last peak history
    bool nowIsAPeak;
    long peakFrameCounter;
    
    /// nframes available for countdown
    long peakCountDuration;
    
    /// control output
    float currentTarget;
};

#endif // OPENAV_DSP_SIDECHAIN_GAIN_H
