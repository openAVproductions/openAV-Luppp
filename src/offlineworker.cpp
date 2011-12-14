
#include "offlineworker.hpp"

#include "top.hpp"
#include "audiobuffer.hpp"

using namespace std;

OfflineWorker::OfflineWorker(Top* t)
{
  top = t;
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

