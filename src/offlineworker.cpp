
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

#include "offlineworker.hpp"

#include "top.hpp"
#include "effect.hpp"
#include "lvtwohost.hpp"
#include "beatsmash.hpp"
#include "trancegate.hpp"
#include "ladspahost.hpp"
#include "audiotrack.hpp"
#include "audiobuffer.hpp"
#include "waveformcache.hpp"

#include "g_statestore.hpp"

#include <sstream>

#include "whitenoiseaudiosource.hpp"
#include "fluidsynthaudiosource.hpp"

// for reading sample pack info stuffs
#include <libconfig.h++>

using namespace std;
using namespace libconfig;

OfflineWorker::OfflineWorker(Top* t)
{
  top = t;
}

std::string OfflineWorker::toString( int in )
{
  std::stringstream stream;
  stream << in;
  return stream.str();
}

// called by GUI thread, when we want to create a new Effect instance.
// A pointer to the Effect is then loaded into the List of Effects in Engine
int OfflineWorker::createNewEffect(int t, int pos, int typeInt )
{
  EffectType type = static_cast<EffectType>(typeInt);
  cout << "OfflineWorker::createNewEffect() type = " << type << endl;
  
  Effect* effect = 0;
  
  switch ( type )
  {
    // all Ladspa effects have the same code here
    case EFFECT_REVERB:
    case EFFECT_LOWPASS:
    case EFFECT_LIMITER:
    case EFFECT_HIGHPASS:
    case EFFECT_TRANSIENT:
    case EFFECT_COMPRESSOR:
    case EFFECT_PARAMETRIC_EQ:
      effect = new LadspaHost(top, type, top->samplerate);
      break;
    case EFFECT_BEATSMASH:
      effect = new BeatSmash(top, type);
      break;
    case EFFECT_TRANCEGATE:
      effect = new TranceGate(top, type);
      break;
    case EFFECT_TESTTONES:
      effect = new Lv2Host(top, type, top->bufferSize, top->samplerate);
      break;
    default: return -1;
  }
  
  if ( effect )
  {
    //cout << "OfflineWorker::createNewEffect() writing EE_STATE_NEW_EFFECT" << endl;
    EngineEvent* x = new EngineEvent();
    x->setStateEffect(t,pos,typeInt,effect);
    top->toEngineQueue.push(x);
  }
  
}

int OfflineWorker::setTrackSource( int t, AudioSourceType type )
{
  cout << "OfflineWorker::setTrackSource() " << t << ", type = " << type << endl;
  
  AudioSource* source = 0;
  
  switch ( type )
  {
    case AUDIO_SOURCE_TYPE_BUFFER    : break; //source = new BufferAudioSource();
    case AUDIO_SOURCE_TYPE_LV2       : break; //source = new BufferAudioSource();
    case AUDIO_SOURCE_TYPE_FLUIDSYNTH: source = new FluidSynthAudioSource(top,"newNameHere"); break;
    case AUDIO_SOURCE_TYPE_WHITENOISE: source = new WhiteNoiseAudioSource(top); break;
    default: cout << "Unknown AudioSourceType recieved!" << endl; break;
  }
  
  EngineEvent* x = new EngineEvent();
  x->setTrackSource(t,(void*)source);
  top->toEngineQueue.push(x);
  
  return 0;
}

// this function is called by the GUI thread, and will create an AudioTrack
// instance, as well as all other nessiary Engine components to have a working
// track. It will then pass pointers to each instance to Engine, which will insert
// them into the Engine's lists.
int OfflineWorker::addTrack(int trackID)
{
  // to be sent to Mixer
  AudioTrack* audiotrack = new AudioTrack(top);
  
  //BufferAudioSource
  BufferAudioSourceState* bas = new BufferAudioSourceState();
  bas->index = 0.f;
  bas->speed = 1.f;
  bas->bufferID = -1;
  
  // ClipSelector
  ClipSelectorState* c = new ClipSelectorState();
  c->ID = -1;
  for ( int i = 0; i < 10; i++ )
  {
    c->clipInfo.push_back( ClipInfo() );
  }
  
  // TrackOutput
  TrackOutputState* tos = new TrackOutputState();
  tos->ID = trackID;
  tos->selected = false;
  tos->volume = 0.f;
  tos->pan = 0.f;
  tos->panZ = 0.f;
  
  //cout << "OfflineW sending ADD TRACK pointers NOW!" << endl;
  EngineEvent* x = new EngineEvent();
  x->sendAddTrackPointers( (void*)audiotrack,(void*)bas,(void*)c,(void*)tos);
  top->toEngineQueue.push(x);
  
}

// called by GUI thread, to load an audiofile to a buffer. The buffer
// is then passed by pointer to Engine, and its unique ID allows its
// use everywhere in Engine
int OfflineWorker::loadAudioBuffer( int ID, int block, std::string name)
{
  // Stage one: Create soundfile handle
  SndfileHandle infile( name , SFM_READ,  SF_FORMAT_WAV | SF_FORMAT_FLOAT , 1 , 44100);
  
  // Stage two: Attempt to read the samples "info" from the .cfg file.
  //  It stores sample lenght (beats) and other info, if no config file
  //  guess the info based on infile.frames() above
  
  std::string dirName = Glib::path_get_dirname ( name );
  std::string baseName= Glib::path_get_basename( name );
  
  std::string configFileName = Glib::build_filename(dirName, "lupppSamplePack.cfg");
  
  //cout << "Looking for samplePack file here: " << configFileName  << endl;
  int sampleNumBeats = 4;
  
  // 1. Check the existence of lupppSamplePack.cfg
  bool fileExists = Glib::file_test ( configFileName , Glib::FILE_TEST_EXISTS );
  if ( fileExists )
  {
    //cout << "Found Config file, reading now!" << endl;
    libconfig::Config config;
    
    try
    {
      config.readFile ( configFileName.c_str() );
    }
    catch(const ParseException &pex)
    {
      std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
    }
    
    int numSamples = -1;
    try
    {
      if ( config.lookupValue("luppp.samplePack.numSamples", numSamples) )
      {
        int numBeats = -1;
        std::string sampleName;
        
        for ( int i = 0; i < numSamples; i++ )
        {
          cout << "Checking sample " << i << "!" << endl;
          if( config.lookupValue( "luppp.samplePack.s"+ toString(i) +".numBeats" , numBeats) &&
              config.lookupValue( "luppp.samplePack.s"+ toString(i) +".name"     , sampleName) )
          {
            if ( sampleName.compare( baseName ) == 0 ) // we have the same sample filename
            {
              cout << "Sample " <<  sampleName << " has " << numBeats << " beats." << endl;
              sampleNumBeats = numBeats;
            }
            else
            {
              // sampleName isn't our current one
            }
          }
          else
          {
            // error finding samplePack.s<i>.numBeats or .name
            cout << "Error finding luppp.samplePack.s"+ toString(i) +".name" << endl;
          }
        } // for loop
      }
    }
    catch ( ParseException &e )
    {
      // some libconfig error, like ParseException, or SettingException
      cout << "OfflineWorker::loadAudioBuffer() LibConfig Parsing exception... ignoring" << endl; 
    }
  }
  else
  {
    cout << "Could not find a 'lupppSamplePack.cfg' file in dir, guessing info!" << endl;
    int FPB = (int) top->samplerate / ( top->bpm / 60.0);
    int numBeatsByDivision = infile.frames() / FPB;
    cout << "Frames = " << infile.frames() << " FPB: " << FPB
         << " numBeats by division: " << numBeatsByDivision << endl;
    sampleNumBeats = numBeatsByDivision;
  }
  
  int size  = infile.frames();
  
  if ( size == 0 )
  {
    std::cout << "OfflineWorker() Could not load sound file, or empty file detected! Check that " << name << " exists!" << std::endl;
    return -1;
  }
  
  // set the samples name to the GUI state, so GUI shows the sample name
  // in the clip selector
  top->guiState->addAudioBufferName( ID, Glib::path_get_basename(name) );
  
  int tmpChan= infile.channels();
  int tmpSR  = infile.samplerate();
  
  // initialize new buffer to hold <size> elements
  std::vector<float> sampleBuffer;
  sampleBuffer.resize(size);
  
  infile.read( &sampleBuffer.at(0) , size );
  
  if ( tmpChan != 1 )
  {
    std::vector<float> tmpVec;
    for(int i = 0; i < size; i++)
    {
      if ( i % tmpChan == 0 )
      {
        tmpVec.push_back(sampleBuffer[i]);
      }
    }
    // swap out all samples except 1 channel
    sampleBuffer.swap( tmpVec );
  }
  
  
  // here we request a shared_ptr to a new AudioBuffer instance from
  // the GUI statestore. Basically the current thread creates the instance
  // over in the GuiStateStore class, and in doing so it gets registered
  // automatically in the GUI part of the engine.
  //
  // When we send the pointer to the Engine, it will set the use count to 2
  // and that will make GUI not delete it. When Engine loses its link to the
  // buffer, the use count will drop and GUI thread will remove it from memory.
  AudioBuffer* buffer = top->guiState->getNewAudioBuffer();
  
  buffer->setBeats(sampleNumBeats);
  
  // swap buffer with the loaded one
  sampleBuffer.swap( *buffer->getPointer() );
  
  std::cout << "Loaded file " << name << " successfully! (Frames = " <<
    buffer->getPointer()->size() << " )  Stored in AudioBuffer ID " << buffer->getID() << std::endl;
  
  // send new AudioBuffer event to engine State
  EngineEvent* x = new EngineEvent();
  x->setStateAudioBuffer( (void*) buffer);
  top->toEngineQueue.push(x);
  
  // send LooperLoad event
  x = new EngineEvent();
  x->looperLoad( ID, block, buffer->getID() );
  top->toEngineQueue.push(x);
  
  // analyse the loaded sample, and write a "waveform cache" audioBuffer.
  analyseBuffer( buffer->getID() );
  
  return 0;
}


void OfflineWorker::analyseBuffer(int ID)
{
  cout << "OfflineWorker::analyseBuffer() ID = " << ID << endl;
  
  // get the relevant AudioBuffer and vector pointer
  AudioBuffer* buffer = top->guiState->getAudioBufferPointer(ID);
  if ( buffer == 0 ) {
    cout << "OfflineWorker::analyseBuffer() ID not valid!" << endl;
    return;
  }
  std::vector<float>* bufferPointer = buffer->getPointer();
  
  // get a new ( or existing ) WaveformCache
  WaveformCache* waveformCache = top->guiState->getWaveformCache(ID);
  if ( waveformCache == 0 ) {
    cout << "Creating new WaveformCache with id " << ID << endl;
    waveformCache = top->guiState->getNewWaveformCache();
    waveformCache->setID(ID);
    waveformCache->setBeats( buffer->getBeats() );
  }
  
  cout << "Input audioBuffer.size() " << bufferPointer->size() << endl;
  
  float currentMax = 0.f;
  bool aboveZero = false;
  
  for (long index=0; index <(long)bufferPointer->size(); index++ )
  {
    float currentSample = bufferPointer->at(index);
    
    if ( currentSample > 0 && currentMax < currentSample ) // top
    {
      currentMax = currentSample;
      aboveZero = true;
    }
    
    // if we cross to below 0, we push back the previous MAX
    if ( currentSample < 0.f && aboveZero == true )
    {
      waveformCache->getPointer()->push_back( currentMax );
      aboveZero = false;
      currentMax = 0.f;
    }
    
  }
  
  // hack to make all the audio data appear in the waveformCache
  // *waveformCache->getPointer() = *bufferPointer;
  
  
}




