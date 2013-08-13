
#ifndef LUPPP_WORKER_H
#define LUPPP_WORKER_H

// Library
#include <string>

#include <sndfile.hh>

#include "audiobuffer.hxx"

using namespace std;

namespace Worker
{
  /// loads a sample into a new AudioBuffer, returning the buffer
  static AudioBuffer* loadSample( string path )
  {
    SndfileHandle infile( path, SFM_READ );
    
    AudioBuffer* ab = new AudioBuffer();
    
    std::vector<float> buf( infile.frames(), 0.f );
    
    infile.read( &buf[0] , infile.frames() );
    
    // read data from file
    ab->setBeats(4);
    ab->nonRtSetSample( buf );
    
    cout << "Worker: loadSample() " << path << " size: " << infile.frames() << endl;
    
    if ( infile.frames() > 0 )
      return ab;
    
    return 0;
  }
  
}

#endif // LUPPP_WORKER_H
