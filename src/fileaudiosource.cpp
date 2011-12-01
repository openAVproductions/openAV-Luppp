
#include "fileaudiosource.hpp"

using namespace std;

int FileAudioSource::privateID = 0;

FileAudioSource::FileAudioSource(Top* t, std::string name)
{
  top = t;
  
  ID = privateID++;
  
  if (ID == 0)
    name = "sample.wav";
  else if ( ID == 1 )
    name = "sample2.wav";
  
  SndfileHandle infile( name , SFM_READ,  SF_FORMAT_WAV | SF_FORMAT_FLOAT , 1 , 44100);

  int size  = infile.frames();

  if ( size == 0 )
  {
    std::cout << "FileAudioSource() " << ID << ":  Could not load sound file, or empty file detected!" << std::endl;
    return;
  }
  
  //int tmpChan= infile.channels();
  int tmpSR  = infile.samplerate();

  //std::cout << "Sampler sample info: size:" << size << ", sr:" << tmpSR << std::flush;

  // initialize new buffer to hold <size> elements
  sampleBuffer.resize(size);

  //std::cout << "\t sampleBuffer->resize("<<size<<")"<<std::endl;

  infile.read( &sampleBuffer.at(0) , size );
}

void FileAudioSource::process (int nframes, float* buffer )
{
  std::list<FileAudioSourceState>::iterator iter = top->state.fileAudioSourceState.begin();
  std::advance(iter, ID);
  
  // read values from state
  float index = iter->index;
  float speed = iter->speed;
  
  //cout << "ID: " << ID << "  speed = " << speed << endl;
  
  // playback the sample
  int size = sampleBuffer.size();
  
  if ( size == 0 )
    return;
  
  for (int i = 0; i < nframes; i++ )
  {
    if ( (int)(index*size) >= size )
    {
      //std::cout << "Index = 0" << std::endl;
      index = 0.f;
    }
    
    *buffer++  += sampleBuffer.at( (int)(index * size) );
    index = index + ( speed / size );
  }
  
  // write values to state
  iter->index = index;
  iter->speed = speed;
  
  //std::cout << "FAS: process() " << index << "  nframes: " << nframes << std::endl;
}
