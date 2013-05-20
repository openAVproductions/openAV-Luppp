
#ifndef LUPPP_WORKER_H
#define LUPPP_WORKER_H

// Library
#include <string>

#include <sndfile.hh>

#include "audiobuffer.hxx"

using namespace std;

namespace Worker
{
  static AudioBuffer* loadSample( string path )
  {
    SndfileHandle infile( path, SFM_READ );
    
    cout << "Worker: loadSample() TODO: fix memory leak using Shared()" << endl;
    AudioBuffer* ab = new AudioBuffer();
    
    std::vector<float> buf( infile.frames(), 0.f );
    
    infile.read( &buf[0] , infile.frames() );
    
    // read data from file
    ab->setBeats(4);
    ab->nonRtSetSample( buf );
    
    cout << "Worker: loadSample() " << path << " size: " << infile.frames() << endl;
    return ab;
  }
}

#endif // LUPPP_WORKER_H
