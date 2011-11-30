
#include "fileaudiosource.hpp"

FileAudioSource::FileAudioSource(Top* t,std::string name)
{
  top = t;
  
  SndfileHandle infile( name , SFM_READ,  SF_FORMAT_WAV | SF_FORMAT_FLOAT , 1 , 44100);

  int size  = infile.frames();

  if ( size == 0 )
  {
    std::cout << "Could not load sound file, or empty file detected!" << std::endl;
    return;
  }
  
  //int tmpChan= infile.channels();
  int tmpSR  = infile.samplerate();

  std::cout << "Sampler sample info: size:" << size << ", sr:" << tmpSR << std::flush;

  // initialize new buffer to hold <size> elements
  sampleBuffer.resize(size);

  std::cout << "\t sampleBuffer->resize("<<size<<")"<<std::endl;

  infile.read( &sampleBuffer.at(0) , size );
}

void FileAudioSource::process (int nframes, float* buffer )
{
  std::list<FileAudioSourceState>::iterator iter = top->fileAudioSourceState.begin();
  
  float index = iter->index;
  float speed = iter->speed;
  
  // playback the sample
  int size = sampleBuffer.size();
  
  if ( size == 0 ) 
    return;
  
  for (int i = 0; i < nframes; i++ )
  {
    if ( (int)index * size >= size )
    {
      std::cout << "Index = 0" << std::endl;
      index = 0.f;
    }
    
    *buffer++ = sampleBuffer.at(index * size);
    index = index + ( speed / size );
  }
  std::cout << "FAS: process() " << index << "  nframes: " << nframes << std::endl;
}
