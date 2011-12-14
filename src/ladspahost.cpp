
#include "ladspahost.hpp"

#include "top.hpp"

using namespace std;

LadspaHost::LadspaHost(Top* t,EffectType et, int s) : Effect(t)
{
  ID = Effect::getID();
  std::cout << "LadspaHost()  Effect::getID() = " << ID << std::endl;
  
  top = t;
  samplerate = s;
  
  hasRunAdding = false;
  
  type = et;
  active = 1;
  
  descriptor = 0; // ladspa_descriptor*
  
  for (int i = 0; i < 255; i++)
  {
    controlBuffer.push_back(0.0);
  }
  
  outputBuffer.resize(1024);
  
  switch(et)
  {
    case EFFECT_REVERB: setPluginByString("/usr/lib/ladspa/g2reverb.so"); break;
    case EFFECT_TRANSIENT:  setPluginByString("/usr/lib/ladspa/transient_1206.so"); break;
    case EFFECT_PARAMETRIC_EQ:  setPluginByString("/usr/lib/ladspa/filters.so"); break;
    case EFFECT_LOWPASS:  setPluginByString("/usr/lib/ladspa/lowpass_iir_1891.so"); break;
    case EFFECT_HIGHPASS:  setPluginByString("/usr/lib/ladspa/highpass_iir_1890.so"); break;
  }
}

void LadspaHost::setParameter(int param, float value)
{
  //std::cout << "LadspaHost::setParameter()  plugin = " << pluginString << std::endl;
  
  // HACK! There's a difference between OSC targets & ladspa port indexs,
  // so we need to convert between the two, right now were just writing
  // the values to the ports we know they should be
  
  int inputPortIndex = param;
  
  
  switch(type)
  {
    case EFFECT_REVERB: { inputPortIndex += 4; } break;
    case EFFECT_LOWPASS: break;
    case EFFECT_HIGHPASS: break;
    case EFFECT_TRANSIENT:
    {
      if ( param == 0 ) inputPortIndex = 2;
      if ( param == 4 ) inputPortIndex = 3;
      break;
    }
    case EFFECT_PARAMETRIC_EQ:
    {
      switch (param)
      {
        case 0: inputPortIndex = 7; break; // g1
        case 1: inputPortIndex =11; break; // g2
        case 2: inputPortIndex =15; break; // g3
        case 3: inputPortIndex =19; break; // g4
        case 4: inputPortIndex = 5; break; // f1
        case 5: inputPortIndex = 9; break; // f2
        case 6: inputPortIndex =13; break; // f3
        case 7: inputPortIndex =17; break; // f4
        default: break;
      }
      break;
    }
  }
  
  // convert the range for the parameter in question
  ladspaPortRangeHint hintDescriptor = descriptor->PortRangeHints[inputPortIndex].HintDescriptor;
  
  float lowerBound;
  float upperBound;
  
  // Lower bound
  if (LADSPA_IS_HINT_BOUNDED_BELOW( hintDescriptor ) )
  {
    lowerBound = descriptor->PortRangeHints[inputPortIndex].LowerBound;
    std::cout << " BelowBound: " << lowerBound << std::endl;
  }
  // Higher bound
  if (LADSPA_IS_HINT_BOUNDED_ABOVE( hintDescriptor ) )
  {
    upperBound = descriptor->PortRangeHints[inputPortIndex].UpperBound;
    std::cout << " UpperBound: " << upperBound << std::endl;
  }
  
  float scaledValue = value * (upperBound - lowerBound) + lowerBound;
  
  std::cout << "LadspaHost::setParameter() after port check & scale, port = " << inputPortIndex << ":  value" << scaledValue << std::endl;
  controlBuffer[inputPortIndex] = scaledValue;
}

void LadspaHost::loadLibrary( std::string filename )
{
  // This function loads *one* .so file, and returns a ladspaHandle
  // representing the object.
  cout << "LadspaHost loading " << filename << "..." << endl;
  
  libraryHandle = dlopen( &filename[0] , RTLD_NOW );
  
  if (libraryHandle == 0)
  {
    cout << "LadspaHost: ERROR loading file " << filename << "." << endl;
  }
}

void LadspaHost::loadPlugin(int pluginIdFromLibrary)
{
  descriptor = 0;
  LADSPA_Descriptor_Function descriptorFunction;
  
  descriptorFunction = (LADSPA_Descriptor_Function) dlsym( libraryHandle, "ladspa_descriptor");
  
  // check that the .so file contains a "ladspa_descriptor" symbol,
  // otherwise it's not a LADSPA plugin, but an .so with a different purpose!
  if (!descriptorFunction)
  {
    const char * pcError = dlerror();
    
    if (pcError)
    {
      cout<< "Unable to find ladspa_descriptor() function in plugin library selected file!\n\
          Are you sure this is a LADSPA plugin file?\n" << endl;
      return;
    }
  }
  
  // cycle trough all available plugins in library, printing thier info
  for (int lIndex = 0; (descriptor = const_cast<ladspaDescriptor>(descriptorFunction(lIndex) ) ) != NULL ; lIndex++)
  { 
    std::cout << descriptor->Name << " " << descriptor->UniqueID << " " << descriptor->Label << std::endl;
  }
  
  // FIXME set descriptor to 1st plugin by default:
  descriptor = const_cast<ladspaDescriptor>( descriptorFunction(pluginIdFromLibrary) );
  
}

void LadspaHost::instantiatePlugin()
{
  cout << "LadspaHost::instantiatePlugin   samprate = " << samplerate << endl;
  
  if (descriptor == 0)
    std::cout << "Descriptor == 0!" << std::endl;
  
  pluginHandle = 0;
  
  // here we create the actual plugin instance
  pluginHandle = descriptor->instantiate( descriptor ,samplerate );
  
  if (!pluginHandle )
  {
    std::cout << "ERROR: LadspaHost::pluginHandle == 0" << std::endl;
    return;
  }
  
  if (!descriptor)
  {
    std::cout << "ERROR: LadspaHost::Descriptor == 0" << std::endl;
    return;
  }
}

void LadspaHost::setPluginByString(std::string inPluginString)
{
  std::cout << "LadspaHost::setPluginByString() Loading .so file:\n" << std::flush;
  
  pluginString = inPluginString;
  
  // Here we get a "ladspaHandle" to the selected .so file 
  loadLibrary ( pluginString );
  
  cout << "loaded library" << endl;
  
  // the ladspa handle used is a class member. no need to pass around!
  loadPlugin (0);
  cout << "loaded plugin" << endl;
  
  // instantiate the plugin
  instantiatePlugin();
  
  // set default values based on which plugin is being loaded
  resetParameters();
  
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
    // lowpass ports
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
  
  // tell the guys what's up
  std::cout << "Loading LADSPA plugin: " << descriptor->Name << "\tAudioPorts:"<<numAudio<<" ControlPorts:"<< numControl<< std::endl;
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
    controlBuffer[5] = 200;
    controlBuffer[6] = 1;
    controlBuffer[7] = 0;
    
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
  
  if ( type == EFFECT_LOWPASS )
  {
    controlBuffer[0] = 440;
    controlBuffer[1] = 2;
  }
  
  if ( type == EFFECT_HIGHPASS )
  {
    controlBuffer[0] = 440;
    controlBuffer[1] = 2;
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
    std::cout << "PluginHandle == 0!!" << std::endl;
    return;
  }
  
  if ( !active )
  {
    std::cout << "." << std::endl;
    return;
  }
  
  if ( type == EFFECT_REVERB )
  {
    descriptor -> connect_port ( pluginHandle , 0  , buffer );
    descriptor -> connect_port ( pluginHandle , 1  , buffer );
    descriptor -> connect_port ( pluginHandle , 2 , &outputBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 3 , &outputBuffer[0] );
    
    descriptor -> connect_port ( pluginHandle , 4, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 5, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 6, &controlBuffer[2] );
    descriptor -> connect_port ( pluginHandle , 7, &controlBuffer[3] );
    descriptor -> connect_port ( pluginHandle , 8, &controlBuffer[4] );
    descriptor -> connect_port ( pluginHandle , 9, &controlBuffer[5] );
    descriptor -> connect_port ( pluginHandle ,10, &controlBuffer[6] );
  }
  else if ( type == EFFECT_LOWPASS )
  {
    // lowpass ports
    float freq = 200 + (70 * pow( 2.0, ((double)(top->state.cutoff*127) - 69.0) / 12.0 ) * 4);
    controlBuffer[0] = freq;
    descriptor -> connect_port ( pluginHandle , 0, &controlBuffer[0] );
    descriptor -> connect_port ( pluginHandle , 1, &controlBuffer[1] );
    descriptor -> connect_port ( pluginHandle , 2, buffer );
    descriptor -> connect_port ( pluginHandle , 3, &outputBuffer[0] );
  }
  else if ( type == EFFECT_HIGHPASS )
  {
    // highpass ports
    float freq = (70 * pow( 2.0, ((double)(top->state.cutoff*127) - 69.0) / 12.0 ) * 3);
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
  
  // run_adding may go wrong for some plugins, check it!
  if ( hasRunAdding )
  {
    std::cout << "RUnadding" << std::endl;
    descriptor->run_adding( pluginHandle , nframes);
  }
  else
  {
    descriptor->run( pluginHandle , nframes);
    
    for ( int i = 0; i < nframes; i++ )
    {
      buffer[i] = outputBuffer[i];
    }
  }
}

LadspaHost::~LadspaHost()
{
  descriptor->cleanup(pluginHandle);
  
  dlclose(libraryHandle);
  
  std::cout << "~LadspaHost() destroyed" << std::endl;
}

