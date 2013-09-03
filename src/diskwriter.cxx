
#include "diskwriter.hxx"

#include <iostream>

using namespace std;

DiskWriter::DiskWriter()
{
  session     = cJSON_CreateObject();
  audioConfig = cJSON_CreateObject();
};

void DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab )
{
  // add the track / scene / name combo to session JSON node
  

  cJSON* clip = cJSON_CreateObject();
  cJSON_AddItemToObject(session, "clip", clip );
  
  cJSON_AddNumberToObject(clip,"track", track);
  cJSON_AddNumberToObject(clip,"scene", scene);
  cJSON_AddStringToObject(clip,"file",    "filenameHere.wav");
  
  
  // write the AudioBuffer contents to <path>/samples/  as  <name>.wav
  // or alternatively t_<track>_s_<scene>.wav
  
}

void DiskWriter::writeSession( std::string path, std::string sessionName )
{
  // write the audioConfig JSON node to <path>/samples/sample.cfg
  
  
  // write session.luppp JSON node to <path>/<sessionName>.luppp
  cJSON_AddItemToObject  ( session, "session", cJSON_CreateString( sessionName.c_str() ));
  cJSON_AddNumberToObject( session, "version_major", 1 );
  cJSON_AddNumberToObject( session, "version_minor", 0 );
  cJSON_AddNumberToObject( session, "version_patch", 0 );
  
  
  cJSON_AddNumberToObject( session, "bpm", 120 );
  char* out = cJSON_Print( session );
  cout << out << endl;
}
