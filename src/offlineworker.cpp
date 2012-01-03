
#include "offlineworker.hpp"

#include "top.hpp"
#include "effect.hpp"
#include "beatsmash.hpp"
#include "ladspahost.hpp"
#include "audiotrack.hpp"
#include "audiobuffer.hpp"

#include "fluidsynthaudiosource.hpp"

using namespace std;

OfflineWorker::OfflineWorker(Top* t)
{
  top = t;
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
    case EFFECT_HIGHPASS:
    case EFFECT_TRANSIENT:
    case EFFECT_COMPRESSOR:
    case EFFECT_PARAMETRIC_EQ:
      effect = new LadspaHost(top, type, top->samplerate);
      break;
    case EFFECT_BEATSMASH:
      effect = new BeatSmash(top, type);
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
  cout << "OfflineWorker::setTrackSource() " << t << ", " << type << endl;
  
  AudioSource* source = 0;
  
  switch ( type )
  {
    case AUDIO_SOURCE_TYPE_BUFFER    : break; //source = new BufferAudioSource();
    case AUDIO_SOURCE_TYPE_LV2       : break; //source = new BufferAudioSource();
    case AUDIO_SOURCE_TYPE_FLUIDSYNTH: source = new FluidSynthAudioSource(top,"newNameHere"); break;
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
  SndfileHandle infile( name , SFM_READ,  SF_FORMAT_WAV | SF_FORMAT_FLOAT , 1 , 44100);
  
  int size  = infile.frames();
  
  if ( size == 0 )
  {
    std::cout << "OfflineWorker() Could not load sound file, or empty file detected!" << std::endl;
    return -1;
  }
  
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
  
  AudioBuffer* buffer = new AudioBuffer();
  
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
  
  return 0;
}

