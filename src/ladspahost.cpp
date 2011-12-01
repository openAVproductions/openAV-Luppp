
#include "ladspahost.hpp"

using namespace std;

int LadspaHost::privateID = 0;

LadspaHost::LadspaHost(EffectType t, int s)
{
  ID = privateID++;
  
  samplerate = s;
  
  type = t;
  active = 1;
  
  descriptor = 0; // ladspa_descriptor*
  
  for (int i = 0; i < 255; i++)
  {
    controlBuffer.push_back(0.0);
  }
  
  outputBuffer.resize(1024);
  
  switch(t)
  {
    case EFFECT_REVERB: setPluginByString("/usr/lib/ladspa/g2reverb.so"); break;
    case EFFECT_TRANSIENT:  setPluginByString("/usr/lib/ladspa/transient_1206.so"); break;
    case EFFECT_PARAMETRIC_EQ:  setPluginByString("/usr/lib/ladspa/filters.so"); break;
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

void LadspaHost::analysePorts()
{
  // caller has mutex
  
  // iterate over all ports
  for ( int i = 0; i < (int)descriptor->PortCount; i++)
  {
    
    std::cout << i << ": " << descriptor->PortNames[i] << " ";
    
    // examine port type
    if (LADSPA_IS_PORT_INPUT  (descriptor->PortDescriptors[i]) != 0)
    {
      numInputs++;
      std::cout << "INPUT";
    }
    if (LADSPA_IS_PORT_OUTPUT (descriptor->PortDescriptors[i]) != 0)
    {
      numOutputs++;
      std::cout << "OUTPUT";
    }
    
    // examine port rate
    if (LADSPA_IS_PORT_CONTROL(descriptor->PortDescriptors[i]) != 0)
    {
      numControl++;
      std::cout << " CONTROL";
      
      // check if its CONTROL & INPUT
      if (LADSPA_IS_PORT_INPUT (descriptor->PortDescriptors[i]) != 0)
        numControlInputs++;
        inputPortIndexVector.push_back(i);
      // check if its CONTROL & OUTPUT
      if (LADSPA_IS_PORT_OUTPUT (descriptor->PortDescriptors[i]) != 0)
        numControlOutputs++;
    }
    if (LADSPA_IS_PORT_AUDIO  (descriptor->PortDescriptors[i]) != 0)
    {
      numAudio++;
      std::cout << " AUDIO";
      
      // check if its AUDIO & INPUT
      if (LADSPA_IS_PORT_INPUT (descriptor->PortDescriptors[i]) != 0)
      {
        std::cout << "Got a AUDIO INPUT!" << std::endl;
        numAudioInputs++;
      }
      // check if its AUDIO & OUTPUT
      if (LADSPA_IS_PORT_OUTPUT (descriptor->PortDescriptors[i]) != 0)
        numAudioOutputs++;
    }
    
    // check range hints
    ladspaData value;
    ladspaPortRangeHint hintDescriptor = descriptor->PortRangeHints[i].HintDescriptor;
    
    // Lower bound
    if (LADSPA_IS_HINT_BOUNDED_BELOW( hintDescriptor ) )
    {
      value = descriptor->PortRangeHints[i].LowerBound;
      
      if (LADSPA_IS_HINT_SAMPLE_RATE( hintDescriptor) && value != 0)
        value = value * samplerate;
      
      std::cout << " BelowBound: " << value;
    }
    // Higher bound
    if (LADSPA_IS_HINT_BOUNDED_ABOVE( hintDescriptor ) )
    {
      value = descriptor->PortRangeHints[i].UpperBound;
      
      if (LADSPA_IS_HINT_SAMPLE_RATE( hintDescriptor ) && value != 0)
        value = value * samplerate;
      
      std::cout << " AboveBound: " << value;
    }
    
    // switch to check defaults:
    ladspaData defaultValue;
    
    switch (hintDescriptor & LADSPA_HINT_DEFAULT_MASK)
    {
      case LADSPA_HINT_DEFAULT_NONE:
        break;
      
      case LADSPA_HINT_DEFAULT_MINIMUM:
        defaultValue = descriptor->PortRangeHints[i].LowerBound;
        if (LADSPA_IS_HINT_SAMPLE_RATE(hintDescriptor) && defaultValue != 0) 
          defaultValue = samplerate * defaultValue;
        
        std::cout << "Default Min:" << defaultValue;
        break;
      
      case LADSPA_HINT_DEFAULT_LOW:
        if (LADSPA_IS_HINT_LOGARITHMIC(hintDescriptor))
        {
          defaultValue = exp(log(descriptor->PortRangeHints[i].LowerBound) * 0.75 + log(descriptor->PortRangeHints[i].UpperBound) * 0.25);
        }
        else
        {
          defaultValue = (descriptor->PortRangeHints[i].LowerBound * 0.75 + descriptor->PortRangeHints[i].UpperBound * 0.25);
        }
        if (LADSPA_IS_HINT_SAMPLE_RATE(hintDescriptor) && defaultValue != 0) 
          defaultValue = samplerate * defaultValue;
        
        std::cout << "Default Low:" << defaultValue;
        break;
      
      case LADSPA_HINT_DEFAULT_MIDDLE:
        if (LADSPA_IS_HINT_LOGARITHMIC(hintDescriptor))
        {
          defaultValue = sqrt(descriptor->PortRangeHints[i].LowerBound * descriptor->PortRangeHints[i].UpperBound);
        }
        else
        {
          defaultValue = 0.5 * (descriptor->PortRangeHints[i].LowerBound + descriptor->PortRangeHints[i].UpperBound);
        }
        if (LADSPA_IS_HINT_SAMPLE_RATE(hintDescriptor) && defaultValue != 0) 
          defaultValue = samplerate * defaultValue;
        
        std::cout << "Default Middle:" << defaultValue;
        
        break;
      
      case LADSPA_HINT_DEFAULT_HIGH:
        if (LADSPA_IS_HINT_LOGARITHMIC(hintDescriptor))
        {
          defaultValue = exp(log(descriptor->PortRangeHints[i].LowerBound) * 0.25 + log(descriptor->PortRangeHints[i].UpperBound) * 0.75);
        }
        else
        {
          defaultValue = (descriptor->PortRangeHints[i].LowerBound * 0.25 + descriptor->PortRangeHints[i].UpperBound * 0.75);
        }
        if (LADSPA_IS_HINT_SAMPLE_RATE(hintDescriptor) && defaultValue != 0) 
          defaultValue = samplerate * defaultValue;
        
        std::cout << "Default High:" << defaultValue;
        break;
      
      case LADSPA_HINT_DEFAULT_MAXIMUM:
        defaultValue = descriptor->PortRangeHints[i].UpperBound;
        if (LADSPA_IS_HINT_SAMPLE_RATE(hintDescriptor) && defaultValue != 0) 
          defaultValue = samplerate * defaultValue;
        
        std::cout << "Default Max:" << defaultValue;
        break;
      
      case LADSPA_HINT_DEFAULT_0:
        cout << ", default 0";
        break;
      
      case LADSPA_HINT_DEFAULT_1:
        cout << ", default 1";
        break;
      
      case LADSPA_HINT_DEFAULT_100:
        cout << ", default 100";
        break;
      
      case LADSPA_HINT_DEFAULT_440:
        cout << ", default 440";
        break;
      
      default:
        cout << ", UNKNOWN DEFAULT CODE";
        /* (Not necessarily an error - may be a newer version.) */
        break;
    }
    
    if (LADSPA_IS_HINT_TOGGLED( hintDescriptor ))
    {
      if (  ( hintDescriptor 
          |  LADSPA_HINT_DEFAULT_0
          |  LADSPA_HINT_DEFAULT_1)
          != (  LADSPA_HINT_TOGGLED 
              | LADSPA_HINT_DEFAULT_0
              | LADSPA_HINT_DEFAULT_1)      )
      {
        cout << ", ERROR: TOGGLED INCOMPATIBLE WITH OTHER HINT";
      }
      else
        cout << ", toggled";
    }
    
    if (LADSPA_IS_HINT_LOGARITHMIC( hintDescriptor ))
      cout << ", logarithmic";
    
    if (LADSPA_IS_HINT_INTEGER( hintDescriptor ))
      cout << ", integer";
    
    /*
    // check for "run_adding()"
    printf("Has run_adding() function: ");
    if (descriptor->run_adding != NULL)
      printf("Yes\n");
    */
    
    ladspaProperties props = descriptor->Properties;
    
    if ( LADSPA_IS_INPLACE_BROKEN( props ) )
    {
      std::cout << "Plugin is not capable of in-place processing and therefore cannot be used by this program.\n" << std::endl;
    }
    
    std::cout <<  std::endl;
  
  } // port iteration loop
  
  std::cout << "Ins:" << numInputs << " Outs:" << numOutputs << " Control:" << numControl << " Audio:" << numAudio << std::endl; 
  
  
  std::cout << "Control Input Port Indexs: " << std::endl;
  for ( int i = 0; i < (int)inputPortIndexVector.size(); i++)
  {
    std::cout << inputPortIndexVector.at(i) << "\t";
  }
  std::cout << std::endl;
  
}

void LadspaHost::instantiatePlugin()
{
  cout << "LadspaHost::instantiatePlugin" << endl;
  
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

void LadspaHost::connectPorts()
{
  // caller has mutex
  
  std::cout << "ConnectPorts()" << std::endl;
  
  int audioCounter = 0;
  int controlCounter = 0;
  
  ladspaPortDescriptor portDescriptor;
  
  int doneOneAudioInputAlready = false;
  int doneOneAudioOutputAlready = false;
  
  for ( int i = 0; i < (int)descriptor->PortCount; i++)
  {
    portDescriptor = descriptor->PortDescriptors[i];
    
    if (LADSPA_IS_PORT_CONTROL(portDescriptor))
    {
      if (LADSPA_IS_PORT_INPUT(portDescriptor))
      {
        std::cout << "Connecting Control Input "<< i << " to controlBuffer " << controlCounter << std::endl;
        controlInputPort = i;
      }
      if (LADSPA_IS_PORT_OUTPUT(portDescriptor))
      {
        std::cout << "Output Control unhandled!" << std::endl;
      }
      
      controlCounter++;
    }
    
    
    if (LADSPA_IS_PORT_AUDIO(portDescriptor))
    {
      if (LADSPA_IS_PORT_INPUT(portDescriptor))
      {
        
        if (numAudioInputs == 1)
        {
          std::cout << "Input AUDIO port number stored " << i << std::endl;
          if ( !doneOneAudioInputAlready )
            inputPortL = i;
          else
            inputPortR = i;
        }
      }
      if (LADSPA_IS_PORT_OUTPUT(portDescriptor))
      {
        std::cout << "Output AUDIO port number stored " << i << std::endl;
        if (numAudioOutputs == 1)
        {
          outputPortL = i;
          outputPortR = i;
        }
        if (numAudioOutputs == 2)
        {
          if ( !doneOneAudioOutputAlready )
            outputPortL = i;
          else
            outputPortR = i;
        }
      }
      
      audioCounter++;
    }
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
  
  // Analyse the plugin ports
  analysePorts();
  
  // connect buffers to ports on plugin
  connectPorts();
  
  // instantiate the plugin
  instantiatePlugin();
  
  // set default values based on which plugin is being loaded
  resetParameters();
  
  // activate the plugin if thats needed
  if (descriptor->activate != 0 )
  {
    std::cout << "ACTIVATING LADSPA PLUGIN..." << std::flush;
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
  descriptor->run_adding( pluginHandle , nframes);
}

LadspaHost::~LadspaHost()
{
  descriptor->cleanup(pluginHandle);
  
  dlclose(libraryHandle);
  
  std::cout << "~LadspaHost() destroyed" << std::endl;
}

