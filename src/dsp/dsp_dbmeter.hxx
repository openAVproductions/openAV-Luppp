
#ifndef OPENAV_DSP_DBMETER_H
#define OPENAV_DSP_DBMETER_H


#include <math.h>
#include <cmath>
#include <cstdlib>

class DBMeter
{
  public:
    DBMeter(int rate)
    {
      fSamplingFreq = rate;
      fConst0 = (96.f / float(min(192000, max(1, fSamplingFreq))));
      
      fRec0[0] = -96.f;
      fRec0[1] = -96.f;
      fRec1[0] = -96.f;
      fRec1[1] = -96.f;
    }
    
    int getNumInputs() { return 2;}
    int getNumOutputs(){ return 2;}
    
    // call these to get the current dB values
    float getLeftDB()
    {
      // range scale from range = -96 -> +10, to 0 -> 1
      float zeroOneL = (1-(fvbargraph0 / -96.f));
      return pow(zeroOneL, 4);
    }
    float getRightDB()
    {
      float zeroOneR = (1-(fvbargraph1 / -96.f));
      return pow(zeroOneR, 4);
    }
    
    void process(int count, float* inputL, float* inputR )
    {
      for (int i = 0; (i < count); i = (i + 1))
      {
        fRec0[0] = max((fRec0[1] - fConst0), min(10.f, (20.f * log10f(max(1.58489e-05f, fabsf(*inputL))))));
        fvbargraph0 = fRec0[0];
        
        fRec1[0] = max((fRec1[1] - fConst0), min(10.f, (20.f * log10f(max(1.58489e-05f, fabsf(*inputR))))));
        fvbargraph1 = fRec1[0];
        
        fRec0[1] = fRec0[0];
        fRec1[1] = fRec1[0];
      }
    }
  
  private:
    float fRec0[2];
    float fRec1[2];
    int fSamplingFreq;
    float fConst0;
    float fvbargraph0;
    float fvbargraph1;
};

#endif // OPENAV_DSP_DBMETER_H

