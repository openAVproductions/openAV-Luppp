
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

#ifndef LUPPP_LV2_HOST
#define LUPPP_LV2_HOST

#include "effect.hpp"

#include <cmath>
#include <limits>
#include <vector>
#include <string>
#include <iostream>

#include <glibmm.h>

// Lilv: Lv2 host library, and extensions
#include <lilv/lilv.h>
#include <lilv/lilvmm.hpp>
#include "lv2/lv2plug.in/ns/ext/event/event.h"
#include "lv2/lv2plug.in/ns/ext/uri-map/uri-map.h"
#include "lv2/lv2plug.in/ns/ext/event/event-helpers.h"


using Glib::RefPtr;

class Top;

class PortDetails : public Glib::Object
{
  public:
    bool input;
    bool output;
    
    bool audio;
    bool event;
    bool control;
    bool midiEvent;
    
    bool logarithmic;
    
    float min;
    float max;
    float def;
    
    PortDetails()
    {
      input = false;
      output = false;
      
      audio = false;
      event = false;
      control = false;
      midiEvent = false;
      
      logarithmic = false;
      
      min = -123.4f;
      max = -123.4f;
      def = -123.4f;
    }
};

class Lv2Host : public Effect
{
  public:
    Lv2Host(Top* t, EffectType, int nframes, int samplerate);
    ~Lv2Host();
    
    void printInfo();
    
    void setParameter(int, float);
    void setParameterAbsolute(int, float);
    
    void loadPlugin(std::string);
    
    void process( int nframes, float *buffer);
  
  protected:
    
    int ID;
    static int privateId;
    
    // General purpose vars
    bool pushChangesToUI;
    
    bool initialized;
    bool portsSuitable;
    
    int nframes;
    int samplerate;
    std::string pluginString;
    
    // Lilv variables
    Lilv::World*  world;
    Lilv::Plugin*  plugin;
    Lilv::Instance*  instance;
    Lilv::Plugins* tmpPluginsList;
    
    // Lv2 functions
    void printNode(Lilv::Node* tmp);
    void infoPlugin(Lilv::Plugin* plugin);
    void printPlugin(Lilv::Plugin* plugin);
    void listPlugins(Lilv::Plugins* pluginsList);
    void getPluginPorts(Lilv::World* world, Lilv::Plugin* plugin);
    
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
    
    int inputPortL;
    int inputPortR;
    int outputPortL;
    int outputPortR;
    int controlInputPort;
    
    int oscNframeCounter;
    
    
    void analysePorts();
    
    void connectPorts(float*);
    
    void checkSuitablility();
    void instantiatePlugin();
    void loadLibrary( std::string filename );
    
    // buffers to pass in / out control / audio data
    std::vector<float> controlBuffer;
    std::vector<float> controlOutputBuffer;
    
    std::vector<float> outputBuffer;
    
    std::vector< RefPtr<PortDetails> > portDetails;
    
    std::map<int,std::string>paramNameMap;
};

#endif

