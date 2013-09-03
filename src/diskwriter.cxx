
#include "diskwriter.hxx"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>

#include "worker.hxx"

using namespace std;

DiskWriter::DiskWriter()
{
  initialize( getenv("HOME"), "sessionName" );
};

void DiskWriter::initialize(std::string path, std::string name )
{
  sessionPath = path;
  sessionName = name;
  
  session = cJSON_CreateObject();
  sample  = cJSON_CreateObject();
  
  // add session metadata
  cJSON_AddItemToObject  ( session, "session", cJSON_CreateString( sessionName.c_str() ));
  cJSON_AddNumberToObject( session, "version_major", 1 );
  cJSON_AddNumberToObject( session, "version_minor", 0 );
  cJSON_AddNumberToObject( session, "version_patch", 0 );
  cJSON_AddNumberToObject( session, "bpm", 120 );
}

void DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab )
{
  // add the track / scene / name combo to session JSON node
  cJSON* clip = cJSON_CreateObject();
  cJSON_AddItemToObject(session, "clip", clip );
  
  cJSON_AddNumberToObject(clip,"track", track);
  cJSON_AddNumberToObject(clip,"scene", scene);
  cJSON_AddStringToObject(clip,"file", "filenameHere.wav");
  
  
  // add the AudioBuffer metadata to the sample JSON node
  cJSON* sampleClip = cJSON_CreateObject();
  cJSON_AddItemToObject(sample, "sample", sampleClip );
  
  stringstream filename;
  filename << "t_" << track << "_s_" << scene << ".wav";
  
  cJSON_AddStringToObject(sampleClip,"file", filename.str().c_str() );
  cJSON_AddNumberToObject(sampleClip,"beats", ab->getBeats() );
  
  // write the AudioBuffer contents to <path>/samples/  as  <name>.wav
  // or alternatively t_<track>_s_<scene>.wav
  
  stringstream path;
  path << sessionPath << "/" << sessionName << "/samples/" << filename.str();
  
  Worker::writeSample( path.str(), ab );
  
  
  
  // de allocate the AudioBuffer here!!
  
}

void DiskWriter::writeSession( std::string path, std::string sessionName )
{
  // write session.luppp JSON node to <path>/<sessionName>.luppp
  stringstream sessionDir;
  sessionDir << getenv("HOME") << "/" << sessionName;
  int sessionDirError = mkdir( sessionDir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  
  if ( sessionDirError )
  {
    // handle by using different filename?
  }
  
  stringstream sampleDir;
  sampleDir << sessionDir.str() << "/samples";
  int sampleDirError  = mkdir( sampleDir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  
  stringstream sessionLuppp;
  sessionLuppp << sessionDir.str() << "/" << sessionName << ".luppp";
  
  //cout << "Session dir: " << sessionDir.str() << "\n" << "Sample dir : " << sampleDir.str() << endl;
  
  ofstream sessionFile;
  sessionFile.open ( sessionLuppp.str().c_str() );
  sessionFile << cJSON_Print( session );
  sessionFile.close();
  
  
  // write the sample JSON node to <path>/samples/sample.cfg
  stringstream sampleConfig;
  sampleConfig << sampleDir.str() << "/sample.cfg";
  
  ofstream sampleFile;
  sampleFile.open ( sampleConfig.str().c_str() );
  sampleFile << cJSON_Print( sample );
  sampleFile.close();
  
  
}
