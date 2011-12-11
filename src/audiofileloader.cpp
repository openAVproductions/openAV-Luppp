
#include "audiofileloader.hpp"

#include "top.hpp"

using namespace std;

AudioFileLoader::AudioFileLoader(Top* t)
{
  top = t;
}

int AudioFileLoader::load( int, int, std::string name)
{
  SndfileHandle infile( name , SFM_READ,  SF_FORMAT_WAV | SF_FORMAT_FLOAT , 1 , 44100);
  
  int size  = infile.frames();
  
  if ( size == 0 )
  {
    std::cout << "AudioFileLoader() Could not load sound file, or empty file detected!" << std::endl;
    return -1;
  }
  
  //int tmpChan= infile.channels();
  int tmpSR  = infile.samplerate();
  
  // initialize new buffer to hold <size> elements
  std::vector<float> sampleBuffer;
  sampleBuffer.resize(size);
  
  infile.read( &sampleBuffer.at(0) , size );
  
  // swap buffer with the loaded one
  AudioBuffer* buffer = new AudioBuffer();
  
  return 0;
}

