
#ifndef LADSPA_HOST
#define LADSPA_HOST

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include <ladspa.h>

class Top;

#include "effect.hpp"
#include <glibmm/module.h>

// define nice names for LADSPA "components"

typedef LADSPA_Data ladspaData;
typedef LADSPA_Handle ladspaHandle;
typedef LADSPA_Properties ladspaProperties;
typedef LADSPA_Descriptor* ladspaDescriptor;
typedef LADSPA_PortDescriptor ladspaPortDescriptor;
typedef LADSPA_PortRangeHintDescriptor ladspaPortRangeHint;

class LadspaHost : public Effect
{
  public:
    LadspaHost(Top*, EffectType type, int samplerate);
    ~LadspaHost();
    
    void setActive(int);
    void setParameter(int, float);
    
    void process(int nframes, float *L);
  
  protected:
    int ID;
    
    Top* top;
    
    EffectType type;
    
    bool hasRunAdding;
    
    int active;
    
    int nframes;
    int samplerate;
    std::string pluginString;
    
    // all port number variables
    int numPorts;
    
    int numInputs;
    int numOutputs;
    
    int numAudio;
    int numControl;
    
    int numAudioInputs;
    int numAudioOutputs;
    
    int numControlInputs;
    int numControlOutputs;
    
    // ladspa handles & functions
    ladspaHandle pluginHandle;
    Glib::Module* ladspaModule;
    ladspaDescriptor descriptor;
    
    int inputPortL;
    int inputPortR;
    int outputPortL;
    int outputPortR;
    int controlInputPort;
    
    void resetParameters();
    
    // buffers to pass in / out control / audio data
    std::vector<float> controlBuffer;
    
    // waste audio buffer
    std::vector<float> outputBuffer;
    
    std::vector<int> inputPortIndexVector;
};

#endif

