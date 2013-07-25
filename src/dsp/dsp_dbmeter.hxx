
#ifndef OPENAV_DSP_DBMETER_H
#define OPENAV_DSP_DBMETER_H


#include <math.h>
#include <cmath>
#include <cstdlib>

float fabsf(float dummy0);
float log10f(float dummy0);

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
    
    // call this to get the current dB value, range = -96 -> +10
    float getLeftDB()
    {
      return fvbargraph0;
    }
    float getRightDB()
    {
      return fvbargraph1;
    }
    
    void process(int count, float** inputs, float** outputs)
    {
      float* input0 = inputs[0];
      float* input1 = inputs[1];
      float* output0 = outputs[0];
      float* output1 = outputs[1];
      for (int i = 0; (i < count); i = (i + 1))
      {
        float fTemp0 = float(input0[i]);
        fRec0[0] = max((fRec0[1] - fConst0), min(10.f, (20.f * log10f(max(1.58489e-05f, fabsf(fTemp0))))));
        fvbargraph0 = fRec0[0];
        output0[i] = float(fTemp0);
        
        float fTemp1 = float(input1[i]);
        fRec1[0] = max((fRec1[1] - fConst0), min(10.f, (20.f * log10f(max(1.58489e-05f, fabsf(fTemp1))))));
        fvbargraph1 = fRec1[0];
        output1[i] = float(fTemp1);
        
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

