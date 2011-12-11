
#include "fileaudiosource.hpp"

#include "audiobuffer.hpp"

using namespace std;

int FileAudioSource::privateID = 0;

FileAudioSource::FileAudioSource(Top* t, std::string name)
{
  top = t;
  
  ID = privateID++;
}

void FileAudioSource::process (int nframes, float* buffer )
{
  // get playback variables
  std::list<FileAudioSourceState>::iterator iter = top->state.fileAudioSourceState.begin();
  std::advance(iter, ID);
  float index = iter->index;
  float speed = iter->speed;
  int audioBufferID = iter->bufferID;
  
  // get AudioBuffer pointer
  AudioBuffer* bufPtr = top->state.getAudioBuffer(audioBufferID);
  if ( bufPtr == 0 ) {
    // happens until a valid file has been loaded for this 
    //cout << "FileAudioSource, bufPtr == 0! returning..." << endl;
    return;
  }
  
  // playback the sample
  int size = bufPtr->getPointer()->size();
  std::vector<float>* tmpVector = bufPtr->getPointer();
  
  if ( size == 0 )
    return;
  
  for (int i = 0; i < nframes; i++ )
  {
    if ( (int)(index*size) >= size )
    {
      //std::cout << "Index = 0" << std::endl;
      index = 0.f;
    }
    
    *buffer++  += tmpVector->at( (int)(index * size) );
    index = index + ( speed / size );
  }
  
  // write values to state
  iter->index = index;
  iter->speed = speed;
  
  //std::cout << "FAS: process() " << index << "  nframes: " << nframes << std::endl;
}
