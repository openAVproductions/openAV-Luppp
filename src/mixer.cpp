
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


int Mixer::setSource(int track, AudioSource* source )
{
  std::list<AudioTrack>::iterator iter =  audiotrackList.begin();
  std::advance(iter, track);
  iter->setSource( source );
  
  return 0;
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

void Mixer::process(int nframes,bool record, PortBufferList& portBufferList)
{
  if ( nframes > 1024 )
  {
    std::cout << "Mixer::process() nframes too large for internal buffers, reduce it!!" << std::endl;
  }
  
  // we push the audio data to a ringbuffer, to be recorded by GUI, and
  // then passed back to Engine as an AudioBuffer*.
  if ( record )
  {
    top->recordAudioQueue.push(nframes, portBufferList.inputAudio);
  }
  
  // process the entire audio chain here
  std::list<AudioTrack>::iterator iter;
  
  for(iter = audiotrackList.begin(); iter != audiotrackList.end(); iter++ )
  {
    iter->process( nframes, &portBufferList.inputAudio[0], &outputW[0], &outputX[0], &outputY[0], &outputZ[0]);
  }
  
  // create a pointer to each *mixer* owned buffer
  float* outPtrW = &outputW[0];
  float* outPtrX = &outputX[0];
  float* outPtrY = &outputY[0];
  float* outPtrZ = &outputZ[0];
  
  bool copyToScopeVector = top->scopeVectorMutex.trylock();
  
  // now sum up the master output buffers and write them
  for(int i = 0; i < nframes; i++)
  {
    // write values to JACK ports
    *portBufferList.outputW++ = *outPtrW;
    *portBufferList.outputX++ = *outPtrX;
    *portBufferList.outputY++ = *outPtrY;
    *portBufferList.outputZ++ = *outPtrZ;
    
    
    if ( copyToScopeVector )
    {
      // write master output value to scopeVector, to be shown in GUI
      top->scopeVector.at(i) = *outPtrW;
      top->inputScopeVector.at(i) = *portBufferList.inputAudio++;
    }
    
    // write 0.f to *mixer* buffer, and increment
    *outPtrW++ = 0.f;
    *outPtrX++ = 0.f;
    *outPtrY++ = 0.f;
    *outPtrZ++ = 0.f;
    
  }
  
  if ( copyToScopeVector )
    top->scopeVectorMutex.unlock();
  
}
