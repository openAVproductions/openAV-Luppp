
#ifndef LUPPP_WORKER_H
#define LUPPP_WORKER_H

// Library
#include <string>

#include <sndfile.hh>

#include "audiobuffer.hxx"

//#include "libjson/libjson.h"

using namespace std;

namespace Worker
{
  /*
  static void writeStuff()
  {
    
    
    JSONNode n(JSON_NODE);
    n.push_back(JSONNode("String Node", "String Value"));
    n.push_back(JSONNode("Integer Node", 42));
    n.push_back(JSONNode("Floating Point Node", 3.14));
    n.push_back(JSONNode("Boolean Node", true));
    std::string jc = n.write_formatted();
    std::cout << jc << std::endl;
    
    
  }
  */
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
