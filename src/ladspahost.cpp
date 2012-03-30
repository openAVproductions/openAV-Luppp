
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ladspahost.hpp"

#include "top.hpp"

#include "effectstate.hpp"

using namespace std;

LadspaHost::LadspaHost(Top* t,EffectType et, int s) : Effect(t, et)
{
  ID = Effect::getID();
  std::cout << "LadspaHost()  Effect::getID() = " << ID << std::endl;
  
  top = t;
  samplerate = s;
  
  hasRunAdding = false;
  
  outputBuffer.resize(1024);
  outputBufferR.resize(1024);
  
  for (int i = 0; i < 255; i++)
  {
    controlBuffer.push_back(0.0);
  }
  
  type = et;
  active = 1;
  
  queueResetParameters = true;
  
  descriptor = 0; // ladspa_descriptor*
  
  pluginSoIndex = 0;
  
  // setup variables for effect type we've been passed
  switch(et)
  {
    case EFFECT_AM_PITCHSHIFT:pluginString = "/usr/lib/ladspa/am_pitchshift_1433.so";break;
    case EFFECT_REVERB:       pluginString = "/usr/lib/ladspa/calf.so"; pluginSoIndex = 3;break;
    case EFFECT_TRANSIENT:    pluginString = "/usr/lib/ladspa/transient_1206.so";   break;
    case EFFECT_PARAMETRIC_EQ:pluginString = "/usr/lib/ladspa/filters.so";          break;
    case EFFECT_LOWPASS:      pluginString = "/usr/lib/ladspa/lowpass_iir_1891.so"; break;
    case EFFECT_HIGHPASS:     pluginString = "/usr/lib/ladspa/highpass_iir_1890.so";break;
    case EFFECT_COMPRESSOR:   pluginString = "/usr/lib/ladspa/sc4m_1916.so";        break;
    case EFFECT_LIMITER:      pluginString = "/usr/lib/ladspa/fast_lookahead_limiter_1913.so"; break;
    case EFFECT_DELAY:        pluginString = "/usr/lib/ladspa/calf.so"; pluginSoIndex = 4; break;
    default: std::cout << "LadspaHost() got unknown effect type!" << std::endl; break;
  }
  
  
  // load the plugin file, using Glib::Module
  bool fileExists = Glib::file_test ( pluginString, Glib::FILE_TEST_EXISTS);
  if ( !fileExists ) {
    cout << " <- file does not exist!" << endl;
    return;
  }
  
  ladspaModule = new Glib::Module( pluginString );
  
  if ( ladspaModule ) {
    cout << "Module loading OK, now getting descriptor function" << endl;
    
    LADSPA_Descriptor_Function descriptorFunction;
    void* tmpFunc;
    bool found = ladspaModule->get_symbol("ladspa_descriptor", tmpFunc );
    
    if ( !found )
    {
      cout << "Could not find LADSPA_Descriptor_Function, returning!" <<endl;
      return;
    }
    
    descriptorFunction = (LADSPA_Descriptor_Function) tmpFunc;
    
    // cycle trough all available plugins in library, printing thier info
    for (int lIndex = 0; (descriptor = const_cast<ladspaDescriptor>(descriptorFunction(lIndex) ) ) != NULL ; lIndex++)
    {
      if ( pluginSoIndex == lIndex )
        std::cout << "All good, plugin info: " << descriptor->Name << " " << descriptor->UniqueID << " " << descriptor->Label << " PluginSoIndex = " << pluginSoIndex << std::endl;
    }
    
    // FIXME set descriptor to 1st plugin by default:
    descriptor = const_cast<ladspaDescriptor>( descriptorFunction( pluginSoIndex ) );
    
  }
  else
  {
    cout << "LadspaHost: ERROR loading file " << pluginString << ", module not found!" << endl;
    return;
  }
  
  // module is loaded, check descriptor & lets instantiate the plugin
  if (descriptor == 0)
  {
    std::cout << "LadspaHost()  Descriptor == 0!" << std::endl;
    return;
  }
  pluginHandle = 0;
  
  // here we create the actual plugin instance
  pluginHandle = descriptor->instantiate( descriptor ,samplerate );
  
  if (!pluginHandle )
  {
    std::cout << "ERROR: LadspaHost::pluginHandle == 0" << std::endl;
    return;
  }
  
  // connect ports, this *must* be done *before* activation for some
  // plugins (Glame HIGH & LOW pass filters)
  if ( type == EFFECT_LOWPASS )
  {
    // lowpass ports
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, &outputBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  if ( type == EFFECT_HIGHPASS )
  {
    // highpass ports
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, &outputBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  
  // activate the plugin if thats needed
  if ( descriptor->activate )
  {
    std::cout << "ACTIVATING LADSPA PLUGIN...  pluginHandle = " << pluginHandle
        << "  Descriptor: " << descriptor << std::flush;
    descriptor-> activate( pluginHandle );
    std::cout << "\t\tDone!" << std::flush;
  }
  
  cout << "LadspaHost() done!" << endl;
}

void LadspaHost::resetParameters()
{
  // here we can compare  pluginString  to hard coded strings to check
  // what plugin were loading, and then set up the controlQueueBuffer to
  // have sane defaults
  
  if ( type == EFFECT_PARAMETRIC_EQ )
  {
    std::cout << "Setting PARAMETRIC defaults NOW!" << std::endl;
    controlBuffer[2] = 1; // turn on master eq switch
    controlBuffer[3] = 0; // master gain
    
    controlBuffer[4] = 1; // turn on section 1 eq switch
    controlBuffer[5] = 200; // freq
    controlBuffer[6] = 1; // bandwidth
    controlBuffer[7] = 0; // gain
    
    controlBuffer[8] = 1; // turn on section 2 eq switch
    controlBuffer[9] = 400;
    controlBuffer[10] = 1;
    controlBuffer[11] = 0;
    
    controlBuffer[12]= 1; // turn on section 3 eq switch
    controlBuffer[13] = 1000;
    controlBuffer[14] = 1;
    controlBuffer[15] = 0;
    
    controlBuffer[16]= 1; // turn on section 4 eq switch
    controlBuffer[17] = 2000;
    controlBuffer[18] = 1;
    controlBuffer[19] = 0;
  }
  else if ( type == EFFECT_DELAY )
  {
    // stereo in, stereo out
    controlBuffer[4] = top->bpm;    // tempo
    controlBuffer[5] = 2;           // subdivide <- primary
    controlBuffer[6] = 1;           // time L
    controlBuffer[7] = 1;           // time R
    
    controlBuffer[8] = 0.9;         // feedback, 0-1, def 0.5
    controlBuffer[9] = 0.8;         // amount
    
    controlBuffer[10] = 0;          // mix mode: 0 = normal, 1 = ping pong
    controlBuffer[11] = 0;          // medium (for delay): 0 = plain, 1 = tape, 2 = old tape
    
    controlBuffer[12] = 1;          // dry amount
    
  }
  else if ( type == EFFECT_AM_PITCHSHIFT )
  {
    controlBuffer[0] = 1; // pitch shift, log
    controlBuffer[1] = 4; // buffer size
    // input audio
    // output audio
    // controlBuffer[4] is latency 
  }
  else if ( type == EFFECT_LOWPASS )
  {
    controlBuffer[0] = 440;
    controlBuffer[1] = 2;
  }
  else if ( type == EFFECT_COMPRESSOR )
  {
    // inputs
    controlBuffer[0] = 1.0;     // RMS = 0, peak = 1
    controlBuffer[1] = 5;     // attack
    controlBuffer[2] = 60.0;    // release
    controlBuffer[3] = 0.0;     // threshold
    controlBuffer[4] = 1.0;     // ratio  1 : 1 - 20
    controlBuffer[5] = 3.25;    // Knee Radius
    controlBuffer[6] = 0.0;     // Makeup
    
    // outputs
    controlBuffer[7] = 0.0; // ampli
    controlBuffer[8] = 0.0; // gain reduc
  }
  else if ( type == EFFECT_LIMITER )
  {
    EffectState* state = top->state.getEffectState(ID);
    state->values[0] = 0.0; // input gain
    state->values[1] = 1.0; // limit
    state->values[2] = 0.1; // release (time in seconds)
  }
  else if ( type == EFFECT_HIGHPASS )
  {
    controlBuffer[0] = 440;
    controlBuffer[1] = 2;
  }
  else if ( type == EFFECT_REVERB )
  {
    // setting values in effect state will cause the values to be set as
    // current default until changed, while writing controlbuffer will
    // result in the value being overwritten. Some values in buffer are
    // not accessible, hence that it is sometimes nessisary to write to it
    EffectState* state = top->state.getEffectState(ID);
    state->values[0] = 0.3; // decay
    state->values[1] = 0.5; // dry wet
    state->values[2] = 1.0; // high damp
    state->values[3] = 0.0; // pre delay
    
    // [0-3] are audio in / out
    controlBuffer[4] = 4.f; // decay time
    controlBuffer[5] = 19000.f; // high freq damp
    controlBuffer[6] = (int)3; // room type (int)
    controlBuffer[7] = 0.2; // diffustion
    controlBuffer[8] = 1.0; // wet
    controlBuffer[9] = 1.0; // dry
    controlBuffer[10]= 0.0; // pre delay
    controlBuffer[11]= 220.0; // bass cut
    controlBuffer[12]= 8000.0; // treb cut
  }
  
}

void LadspaHost::setActive(int a)
{
  std::cout << "LadspaHost::setActive()  " << a << std::endl;
  active = a;
}

void LadspaHost::process(int nframes, float* buffer)
{
  if ( pluginHandle == 0 )
  {
    //std::cout << "PluginHandle == 0!!" << std::endl;
    return;
  }
  
  if ( descriptor == 0 )
  {
    //cout << "LadspaHost::descriptor == 0" << endl;
  }
  
  if ( queueResetParameters )
  {
    // needed as when we create an effect we can't call into the
    // engine statestore, because we're in the GUI thread 
    resetParameters();
    queueResetParameters = false;
  }
  
  if ( !active )
  {
    std::cout << "." << std::endl;
    return;
  }
  
  // ID is the Unique Effect ID, so we always get back this instances state
  EffectState* state = top->state.getEffectState(ID);
  //cout << "ladspahost got effectstate " << state << " from ID " << ID << " value 0 = " << state->values[0] << endl; 
  
  // AutoMove feature, range scale the unit's value here
  float globalChange = 0.f;
  
  switch ( top->state.globalUnitType )
  {
    case AUTOMOVE_TYPE_UP:        globalChange = 1 - state->values[0]; break;
    case AUTOMOVE_TYPE_DOWN:      globalChange =     state->values[0]; break;
    default: break;
  }
  
  if ( !state->active )
  {
    return;
  }
  
  if ( type == EFFECT_REVERB )
  {
    controlBuffer[4] = 0.4 + state->values[0] * 14.6; // decay
    
    float dryWet = state->values[1];
    
    controlBuffer[8] = sin(dryWet * (3.1415/2.f)); // wet
    controlBuffer[9] = cos(dryWet* 1.6); // dry
    if ( controlBuffer[9] < 0.f )
      controlBuffer[9] = 0.f;
    
    controlBuffer[10]= state->values[2] * 50 ; // pre delay
    controlBuffer[5] = state->values[3] * 20000; // high damp
    
    //cout << "Decay: " << controlBuffer[4] << "  Wet: " << controlBuffer[8] << "  Dry: " << controlBuffer[9]
    //     << "  preDelay: " << controlBuffer[10] << "  highDamp " << controlBuffer[5] << endl;
    
    descriptor -> connect_port ( pluginHandle , 0  , buffer );
    descriptor -> connect_port ( pluginHandle , 1  , buffer );
    descriptor -> connect_port ( pluginHandle , 2 , &outputBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 3 , &outputBufferR[0] );
    
    descriptor -> connect_port ( pluginHandle , 4, &controlBuffer[4] );
    descriptor -> connect_port ( pluginHandle , 5, &controlBuffer[5] );
    descriptor -> connect_port ( pluginHandle , 6, &controlBuffer[6] );
    descriptor -> connect_port ( pluginHandle , 7, &controlBuffer[7] );
    descriptor -> connect_port ( pluginHandle , 8, &controlBuffer[8] );
    descriptor -> connect_port ( pluginHandle , 9, &controlBuffer[9] );
    descriptor -> connect_port ( pluginHandle ,10, &controlBuffer[10] );
    descriptor -> connect_port ( pluginHandle ,11, &controlBuffer[11] );
    descriptor -> connect_port ( pluginHandle ,12, &controlBuffer[12] );
  }
  else if ( type == EFFECT_DELAY )
  {
    controlBuffer[4] = top->bpm; // update controls
    controlBuffer[5] = (int)1 + state->values[0] * 7; // primary control = subdivision
    
    controlBuffer[6] = 1;           // time L
    controlBuffer[7] = 1;           // time R
    
    controlBuffer[8] = 0.9;         // feedback, 0-1, def 0.5
    controlBuffer[9] = 0.8;         // amount
    
    controlBuffer[10] = 0;          // mix mode: 0 = normal, 1 = ping pong
    controlBuffer[11] = 0;          // medium (for delay): 0 = plain, 1 = tape, 2 = old tape
    
    controlBuffer[12] = 1;          // dry amount
    
    
    
    cout << "Delay sub = " << controlBuffer[5] << endl;
    
    descriptor -> connect_port ( pluginHandle , 0 , buffer );
    descriptor -> connect_port ( pluginHandle , 1 , buffer );
    descriptor -> connect_port ( pluginHandle , 2 , &outputBuffer[0]  );
    descriptor -> connect_port ( pluginHandle , 3 , &outputBufferR[0] );
    
    descriptor -> connect_port ( pluginHandle , 4, &controlBuffer[4]  );
    descriptor -> connect_port ( pluginHandle , 5, &controlBuffer[5]  );
    descriptor -> connect_port ( pluginHandle , 6, &controlBuffer[6]  );
    descriptor -> connect_port ( pluginHandle , 7, &controlBuffer[7]  );
    descriptor -> connect_port ( pluginHandle , 8, &controlBuffer[8]  );
    descriptor -> connect_port ( pluginHandle , 9, &controlBuffer[9]  );
    descriptor -> connect_port ( pluginHandle ,10, &controlBuffer[10] );
    descriptor -> connect_port ( pluginHandle ,11, &controlBuffer[11] );
    descriptor -> connect_port ( pluginHandle ,12, &controlBuffer[12] );
  }
  else if ( type == EFFECT_AM_PITCHSHIFT )
  {
    controlBuffer[0] = 0.25 + state->values[0] * 3.75;
    descriptor -> connect_port ( pluginHandle , 0 , &controlBuffer[0] ); // pitch
    descriptor -> connect_port ( pluginHandle , 1 , &controlBuffer[1] ); // buffer size
    descriptor -> connect_port ( pluginHandle , 2 , buffer );
    descriptor -> connect_port ( pluginHandle , 3 , &outputBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 4 , &controlBuffer[4] ); // out latency
  }
  else if ( type == EFFECT_LIMITER )
  {
    controlBuffer[0] = state->values[0] * 20; // input gain
    controlBuffer[1] = -20 + (state->values[1] * 20); // limit
    controlBuffer[2] = 0.02; // release (time in seconds, 0.01 -> 2 )
    
    //cout << "LADSPA Limiter: inputGain " << controlBuffer[0] << "  limit: " << controlBuffer[1]
    //     << "  release: " << controlBuffer[2] << "  ReductionAmount " << controlBuffer[3] << " latency " << controlBuffer[8] << endl;
    
    descriptor -> connect_port ( pluginHandle ,  0 , &controlBuffer[0] ); // control inputs
    descriptor -> connect_port ( pluginHandle ,  1 , &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle ,  2 , &controlBuffer[2] );
    
    descriptor -> connect_port ( pluginHandle ,  3 , &controlBuffer[3] ); // attenuation output
    
    descriptor -> connect_port ( pluginHandle ,  4 , buffer ); // audio input
    descriptor -> connect_port ( pluginHandle ,  5 , buffer ); 
    descriptor -> connect_port ( pluginHandle ,  6 ,&outputBuffer[0] ); // audio output
    descriptor -> connect_port ( pluginHandle ,  7 ,&outputBufferR[0] );
    
    descriptor -> connect_port ( pluginHandle ,  8 ,&controlBuffer[8] ); // latency control output
  }
  else if ( type == EFFECT_LOWPASS )
  {
    // lowpass ports
    float finalValue = state->values[0];
    
    if ( state->globalUnit ) // overwrite old value with globalUnit effected one
    {
      finalValue = state->values[0] + globalChange * top->state.globalUnit;
    }
    
    if ( finalValue > 1 )
      finalValue = 1;
    if ( finalValue < 0 )
      finalValue = 0.f;
    
    // update GUI
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setPluginParameter(ID, -1, 0, finalValue);
    top->toGuiQueue.push(x);
    
    float freq = 200 + (70 * pow( 2.0, ((double)( finalValue*127) - 69.0) / 12.0 ) * 5);
    controlBuffer[0] = freq;
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, buffer );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  else if ( type == EFFECT_COMPRESSOR )
  {
    // compressor control values
    float thresh = state->values[0]; // array slice here = knob on controller
    float release= state->values[1];
    float ratio  = state->values[4];
    float makeup = state->values[5];
    
    controlBuffer[2] = 4 + release * 400; // array slice here = which parameter to control
    controlBuffer[4] = ((1-thresh) * -29) - 1;
    controlBuffer[5] = 1 + ratio * 19;
    controlBuffer[6] = makeup * 24;
    
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] ); // RMS / peak ( 0, 1 )
    
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, &controlBuffer[2] ); // release
    descriptor -> connect_port ( pluginHandle , 3, &controlBuffer[3] );
    descriptor -> connect_port ( pluginHandle , 4, &controlBuffer[4] ); // thres
    descriptor -> connect_port ( pluginHandle , 5, &controlBuffer[5] ); // ratio
    descriptor -> connect_port ( pluginHandle , 6, &controlBuffer[6] ); // makeup
    
    descriptor -> connect_port ( pluginHandle , 7, &controlBuffer[7] ); // amplitude output
    descriptor -> connect_port ( pluginHandle , 8, &controlBuffer[8] ); // gain reduction output
    
    descriptor -> connect_port ( pluginHandle , 9, buffer );
    descriptor -> connect_port ( pluginHandle ,10, &outputBuffer[0] );
    
    //std::cout << "T = " << controlBuffer[4] << "\tR = " << controlBuffer[5] << "\tM = " << controlBuffer[6] << " Release: " << controlBuffer[2] << " Comp = " << (int) (controlBuffer[8] / 24.)*100 << "\tamp = " << controlBuffer[7]  << std::endl;
  }
  else if ( type == EFFECT_HIGHPASS )
  {
    // highpass ports
    float finalValue = state->values[0];
    if ( state->globalUnit ) // overwrite old value with globalUnit effected one
    {
      finalValue = state->values[0] + globalChange * top->state.globalUnit;
    }
    
    if ( finalValue > 1 )
      finalValue = 1;
    if ( finalValue < 0 )
      finalValue = 0.f;
    
    // update GUI
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->setPluginParameter(ID, -1, 0, finalValue);
    top->toGuiQueue.push(x);
    
    float freq = (70 * pow( 2.0, ((double)(finalValue*127) - 69.0) / 12.0 ) * 3);
    controlBuffer[0] = freq;
    
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, buffer );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  else if ( type == EFFECT_TRANSIENT )
  {
    // transient ports
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, buffer );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  else if ( type == EFFECT_PARAMETRIC_EQ )
  {
    // parametric
    //cout << "LadspaHost Parametric  value 0 = " << state->values[0] << endl; 
    controlBuffer[07] = (state->values[0]-0.5) * 20; // gains on top 4 knobs
    controlBuffer[11] = (state->values[1]-0.5) * 20;
    controlBuffer[15] = (state->values[2]-0.5) * 20;
    controlBuffer[19] = (state->values[3]-0.5) * 20;
    
    descriptor -> connect_port ( pluginHandle , 0, buffer );
    descriptor -> connect_port ( pluginHandle , 1, &outputBuffer[0] );
    
    descriptor -> connect_port ( pluginHandle , 2, &controlBuffer[2]  );
    descriptor -> connect_port ( pluginHandle , 3, &controlBuffer[3]  );
    descriptor -> connect_port ( pluginHandle , 4, &controlBuffer[4]  );
    descriptor -> connect_port ( pluginHandle , 5, &controlBuffer[5]  );
    descriptor -> connect_port ( pluginHandle , 6, &controlBuffer[6]  );
    descriptor -> connect_port ( pluginHandle , 7, &controlBuffer[7]  );
    descriptor -> connect_port ( pluginHandle , 8, &controlBuffer[8]  );
    descriptor -> connect_port ( pluginHandle , 9, &controlBuffer[9]  );
    descriptor -> connect_port ( pluginHandle ,10, &controlBuffer[10]  );
    descriptor -> connect_port ( pluginHandle ,11, &controlBuffer[11]  );
    descriptor -> connect_port ( pluginHandle ,12, &controlBuffer[12] );
    descriptor -> connect_port ( pluginHandle ,13, &controlBuffer[13] );
    descriptor -> connect_port ( pluginHandle ,14, &controlBuffer[14] );
    descriptor -> connect_port ( pluginHandle ,15, &controlBuffer[15] );
    descriptor -> connect_port ( pluginHandle ,16, &controlBuffer[16] );
    descriptor -> connect_port ( pluginHandle ,17, &controlBuffer[17] );
    descriptor -> connect_port ( pluginHandle ,18, &controlBuffer[18] );
    descriptor -> connect_port ( pluginHandle ,19, &controlBuffer[19] );
  }
  else
  {
    std::cout << "LadspaHost NOT OK: Unknown plugin type!" <<std::endl;
    return;
  }
  
  descriptor->run( pluginHandle , nframes);
  
  for ( int i = 0; i < nframes; i++ )
  {
    buffer[i] = outputBuffer[i];
  }
}

LadspaHost::~LadspaHost()
{
  descriptor->cleanup(pluginHandle);
  std::cout << "~LadspaHost() destroyed" << std::endl;
}

