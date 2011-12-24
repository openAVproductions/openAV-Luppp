
#include "mixer.hpp"

#include "top.hpp"

using namespace std;

Mixer::Mixer(Top* t)
{
  top = t;
  
  // Top isn't initialized yet, so we just initialize all buffers to 1024
  outputW.resize(1024);
  outputX.resize(1024);
  outputY.resize(1024);
  outputZ.resize(1024);
  
  inputBuffer.resize(1024);
}

void Mixer::addTrack(AudioTrack* trackPtr)
{
  //cout << "Mixer::addTrack() AudioTrack* = " << trackPtr << endl;
  audiotrackList.push_back( *(trackPtr) );
}


int Mixer::addEffect(int track, int pos,  Effect* eff )
{
  std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
  std::advance(iter, track);
  iter->addEffect( pos , eff);
  
  return 0;
}

int Mixer::getEffectID(int track, int pos)
{
  if ( track < audiotrackList.size() )
  {
    //cout << "Mixer::getEffectID() t: " << track << " pos: " << pos << endl;
    std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
    std::advance(iter, track);
    return iter->getEffectID( pos );
  }
  else
  {
    cout << "Mixer::getEffectID() OOB! t: " << track << endl;
  }
  
  return -1;
}

void Mixer::process(int nframes,bool record, float* inBuffer, float* outBuffer)
{
  if ( nframes > 1024 )
  {
    std::cout << "Mixer::process() nframes too large for internal buffers, reduce it!!" << std::endl;
  }
  
  // process the entire audio chain here
  std::list<AudioTrack>::iterator iter;
  
  for(iter = audiotrackList.begin(); iter != audiotrackList.end(); iter++ )
  {
    iter->process( nframes, &inputBuffer[0], &outputW[0], &outputX[0], &outputY[0], &outputZ[0] );
  }
  
  float* outPointer = &outputW[0];
  
  if ( record ) // we push the audio data to a ringbuffer
  {
    top->recordAudioQueue.push(nframes, inBuffer);
  }
  
  bool copyToScopeVector = top->scopeVectorMutex.trylock();
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    // copy value
    *outBuffer++ = *outPointer;
    
    if ( copyToScopeVector )
    {
      // write master output value to scopeVector, to be shown in GUI
      top->scopeVector.at(i) = *outPointer;
      top->inputScopeVector.at(i) = *inBuffer++;
    }
    
    // write 0.f to buffer, and increment
    *outPointer++ = 0.f;
    inputBuffer[i] = 0.f;
    
  }
  
  if ( copyToScopeVector )
    top->scopeVectorMutex.unlock();
  
}
