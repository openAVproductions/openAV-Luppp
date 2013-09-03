
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
    
    cJSON *root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "session", cJSON_CreateString( sessionName.c_str() ));
    cJSON_AddNumberToObject( root, "version_major", 1 );
    cJSON_AddNumberToObject( root, "version_minor", 0 );
    cJSON_AddNumberToObject( root, "version_patch", 0 );
    
    
    cJSON_AddNumberToObject( root, "bpm", 120 );
    
    for(int t = 0; t < NTRACKS; t++ )
    {
      cJSON* track = cJSON_CreateObject();
      cJSON_AddItemToObject(root, "track", track );
      
      cJSON_AddNumberToObject(track,"ID", t);
      
      for(int s = 0; s < NSCENES; s++ )
      {
        cJSON* clip = cJSON_CreateObject();
        cJSON_AddItemToObject(track, "clip", clip );
        
        cJSON_AddNumberToObject(clip,"ID", s);
        cJSON_AddStringToObject(clip,"file",    "filenameHere.wav");
      }
    }
    
    char* out = cJSON_Print(root);
    
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
