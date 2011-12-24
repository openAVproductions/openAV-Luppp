
#include "bufferaudiosource.hpp"

#include "audiobuffer.hpp"

using namespace std;

BufferAudioSource::BufferAudioSource(Top* t)
{
  top = t;
  ID = AudioSource::getID();
  //cout << "BufferAudioSource() ID = " << ID << endl;
  
  guiUpdateCounter = 0;
}

void BufferAudioSource::process (int nframes, float* buffer )
{
  // get playback variables
  std::list<BufferAudioSourceState>::iterator iter = top->state.bufferAudioSourceState.begin();
  std::advance(iter, ID);
  float index = iter->index;
  float speed = iter->speed;
  
  // get buffer variables
  ClipSelectorState* clipSelState = top->state.getClipSelectorState(ID);
  if ( !clipSelState )
    return;
  
  int playingScene = clipSelState->playing;
  if ( playingScene < 0 )
    return;
  
  std::list<ClipInfo>::iterator clipIter = clipSelState->clipInfo.begin();
  std::advance(clipIter, playingScene);
  int playingBuffer = (*clipIter).bufferID;
  speed = (*clipIter).speed;
  //std::cout << "FAS:P()   sceneID: " << playingScene << "   bID = " << playingBuffer << endl;
  
  // get AudioBuffer pointer
  AudioBuffer* bufPtr = top->state.getAudioBuffer(playingBuffer);
  if ( bufPtr == 0 ) {
    // happens until a valid file has been loaded for this 
    //cout << "BufferAudioSource, bufPtr == 0! returning..." << endl;
    return;
  }
  
  // playback the sample
  int size = bufPtr->getPointer()->size();
  std::vector<float>* tmpVector = bufPtr->getPointer();
  
  if ( size == 0 )
    return;
  
  //cout << "BAS:process() ID = " << ID << "  Size: " << size << " index = " << index << endl;
  for (int i = 0; i < nframes; i++ )
  {
    if ( (int)(index*size) >= size )
    {
      //cout << "BufferAudioSource:proces()  index*size > size!  vectorSize = " << tmpVector->size() << endl;
      index = 0.f;
    }
    
    *buffer++  += tmpVector->at( (int)(index * size) );
    index = index + ( speed / size );
  }
  
  // write index to state, so that next time we read the next samples
  iter->index = index;
  
  if ( guiUpdateCounter < 0 )
  {
    EngineEvent* x = top->toEngineEmptyEventQueue.pull();
    x->looperProgress(ID, index ); // index is a float 0 -> 1
    top->toGuiQueue.push(x);
    guiUpdateCounter = 20;
  }
  else
  {
    guiUpdateCounter--;
  }
}
