
#include "diskreader.hxx"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

#include "event.hxx"
#include "eventhandler.hxx"
#include "worker.hxx"

using namespace std;

DiskReader::DiskReader()
{
};

void DiskReader::readSession( std::string path )
{
  cout << "DiskReader::readSession() " << path << endl;
  sessionPath = path;
  
  stringstream s;
  s << path << "/session.luppp";
  
  stringstream samplePath;
  samplePath << path << "/samples/sample.cfg";
  
  cout << "session path: " << s.str() << endl;
  cout << "sample path:  " << samplePath.str() << endl;
  
  // open session, read all
  std::ifstream file( s.str().c_str(), std::ios_base::in|std::ios_base::ate);
  long file_length = file.tellg();
  file.seekg(0, std::ios_base::beg);
  file.clear();
  char *sessionString = new char[file_length];
  file.read(sessionString, file_length);
  
  // open sample, read all
  std::ifstream sampleFile( samplePath.str().c_str(), std::ios_base::in|std::ios_base::ate);
  file_length = sampleFile.tellg();
  sampleFile.seekg(0, std::ios_base::beg);
  sampleFile.clear();
  char *sampleString = new char[file_length];
  sampleFile.read(sampleString, file_length);
  
  //cout << "sessionFile string:\n " << sessionString << endl;
  //cout << "sampleFile string: \n " << sampleString << endl;
  
  
  // create cJSON nodes from strings
  session = cJSON_Parse( sessionString );
  if (!session) {
    printf("Error in Session JSON before: [%s]\n",cJSON_GetErrorPtr());
    return;
  }
  sample  = cJSON_Parse( sampleString );
  if (!sample) {
    printf("Error in Sample JSON before: [%s]\n",cJSON_GetErrorPtr());
    return;
  }
  //cout << "readSample: " << cJSON_Print( sample ) << endl;
  
  
  
  cJSON* tracks = cJSON_GetObjectItem( session, "tracks");
  if ( tracks )
  {
    int nTracks = cJSON_GetArraySize( tracks );
    for(int t = 0; t < nTracks; t++ )
    {
      cJSON* track = cJSON_GetArrayItem( tracks, t );
      
      cJSON* clips = cJSON_GetObjectItem( track, "clips");
      if ( clips )
      {
        
        int nClips = cJSON_GetArraySize( clips );
        for(int s = 0; s < nClips; s++ )
        {
          // get metadata for Clip
          cJSON* clip = cJSON_GetArrayItem( clips, s );
          
          if ( !strcmp(clip->valuestring, "") == 0 )
          {
            stringstream sampleFilePath;
            sampleFilePath << path << "/samples/" << clip->valuestring;
#ifdef DEBUG_LOAD
        cout << "clip " << sampleFilePath.str() << endl;
#endif
            
            // load it
            AudioBuffer* ab = Worker::loadSample( sampleFilePath.str() );
            EventLooperLoad e = EventLooperLoad( t, s, ab );
            writeToDspRingbuffer( &e );
            
            // retrieve sample metadata from sample.cfg using filename as key
            cJSON* sampleFile = cJSON_GetObjectItem( sample, clip->valuestring );
            if ( sampleFile )
            {
              cJSON* beats = cJSON_GetObjectItem( sampleFile, "beats" );
              
              cout << "Clip @ " << t << " " << s << " gets " << beats->valuedouble << " beats."<< endl;
              EventLooperLoopLength e = EventLooperLoopLength( t, s, beats->valueint );
              writeToDspRingbuffer( &e );
            }
            else {
              cout << "Wanring: Sample " << clip->valuestring << " has no entry for beats." << endl;
            }
          }
        
        } // nClips loop
      
      }
        
    } // nTracks loop
    
  }
  else
  {
    cout << "DiskReader: Error getting clip" << endl;
  }
  
  // cleanup
  cJSON_Delete( session );
  cJSON_Delete( sample  );
  
  free ( sessionString );
  free ( sampleString  );
  
}
