
#ifndef LUPPP_WORKER_H
#define LUPPP_WORKER_H

// Library
#include <string>

#include <sndfile.hh>

#include "audiobuffer.hxx"

#include "cjson/cJSON.h"

using namespace std;

namespace Worker
{
  
  static void save(std::string path, std::string sessionName)
  {
    cJSON *root, *fmt;
    char* out;
    
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "session", cJSON_CreateString( sessionName.c_str() ));
    cJSON_AddItemToObject(root, "format", fmt=cJSON_CreateObject() );
    
    cJSON_AddStringToObject(fmt,"type",    "rect");
    cJSON_AddNumberToObject(fmt,"width",    1920);
    cJSON_AddNumberToObject(fmt,"height",    1080);
    
    cJSON_AddFalseToObject (fmt,"interlace");
    cJSON_AddNumberToObject(fmt,"frame rate",  24);
    
    cJSON_AddNumberToObject( root, "version", 1 );
    
    out=cJSON_Print(root);
    
    cout << out << endl;
  }
  
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
