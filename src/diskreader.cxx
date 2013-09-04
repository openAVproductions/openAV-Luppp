
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
  // TODO : get from user input
  
  stringstream s;
  s << getenv("HOME") << "/" << "sessionName" << "/";
  readSession( s.str() );
};

void DiskReader::readSession( std::string path )
{
  cout << "DiskReader::readSession() " << path << endl;
  sessionPath = path;
  
  char *sampleChar;
  std::string sessionString;
  
  
  stringstream s;
  s << path << "session.luppp";
  
  cout << "session path " << s.str().c_str() << endl;
  
  
  // open file, store entire contents into string
  std::ifstream file( s.str().c_str(), std::ios_base::in|std::ios_base::ate);
  long file_length = file.tellg();
  file.seekg(0, std::ios_base::beg);
  file.clear();
  char *string = new char[file_length];
  file.read(string, file_length);
  
  cout << "sessionFile string:\n " << string << endl;
  
  
  session = cJSON_Parse( string );
  
  if (!session)
  {
    printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    return;
  }
  
  //cout << "readSession: " << cJSON_Print( session ) << endl;
  
  
  cJSON* clip = cJSON_GetObjectItem( session, "clip");
  if ( clip )
  {
    // get metadata for Clip
    cJSON* track = cJSON_GetObjectItem( clip, "track");
    cJSON* scene = cJSON_GetObjectItem( clip, "scene");
    cJSON* file  = cJSON_GetObjectItem( clip, "file");
    int t = track->valueint;
    int s = scene->valueint;
    stringstream sampleFilePath;
    sampleFilePath << path << "/samples/" << file->valuestring;
#ifdef DEBUG_LOAD
    cout << "clip has track " << t << " scene " << s << "  file: " <<
        sampleFilePath.str() << endl;
#endif
    
    
    
    // load it
    AudioBuffer* ab = Worker::loadSample( sampleFilePath.str() );
    EventLooperLoad e = EventLooperLoad( t, s, ab );
    writeToDspRingbuffer( &e );
  }
  else
  {
    cout << "DiskReader: Error getting clip" << endl;
  }
  
  
  
  /*
  for( int i = 0; i < cJSON_GetArraySize( session ); i++ )
  {
    cJSON* subitem = cJSON_GetArrayItem( session, i);
    
    
    
   name = cJSON_GetObjectItem(subitem, "name");
   index = cJSON_GetObjectItem(subitem, "index");
   optional = cJSON_GetObjectItem(subitem, "optional"); 
    
    cout << "i = " << i << "  session: " << subitem->valueint << endl;
    
  }*/
  
  
  
  cJSON_Delete( session );
  
  
  free ( string );
  
}
