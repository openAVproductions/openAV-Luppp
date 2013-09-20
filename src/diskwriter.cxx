
#include "diskwriter.hxx"

#include "config.hxx"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>

#include "gui.hxx"
#include "gmastertrack.hxx"

#include <sndfile.hh>

extern Gui* gui;

using namespace std;

DiskWriter::DiskWriter()
{
  session = cJSON_CreateObject();
  sample  = cJSON_CreateObject();
  
  sessionPath = getenv("HOME");
  sessionName = "lupppSession";
  
};

void DiskWriter::initialize(std::string path, std::string name )
{
  sessionPath = path;
  sessionName = name;
}

std::string DiskWriter::getLastSaveName()
{
  return sessionName;
}

std::string DiskWriter::getLastSavePath()
{
  return sessionPath;
}

int DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab )
{
  // get the filename
  stringstream filename;
  filename << "t_" << track << "_s_" << scene << ".wav";
  
  // store the clip in clipData, we will write the session JSON for it in writeSession
  clipData.push_back( ClipData( track, scene, filename.str() ) );
  
  // add the AudioBuffer metadata to the sample JSON node
  cJSON* sampleClip = cJSON_CreateObject();
  cJSON_AddItemToObject(sample, filename.str().c_str(), sampleClip );
  cJSON_AddNumberToObject(sampleClip,"beats", ab->getBeats() );
  
  // write the AudioBuffer contents to <path>/samples/  as  <name>.wav
  // or alternatively t_<track>_s_<scene>.wav
  
  // FIXME: trim trailing  /  sessionPath from session path if its there
  
  stringstream path;
  path << sessionPath << "/" << sessionName << "/samples/" << filename.str();
  
  SndfileHandle outfile( path.str(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 1, 44100);
  cout << "Worker::writeSample() " << path.str() << " size: " << ab->getAudioFrames() << endl;
  
  // FIXME: the size of the buffer is bigger than the audio contained in it:
  // calculate the length that needs saving using getBeats() * framesPerBeat
  if ( ab->getAudioFrames() > 0 )
    outfile.write( &ab->getData()[0], ab->getAudioFrames() );
  else
  {
    LUPPP_WARN("%s","Sample has zero samples");
  }
  
  return LUPPP_RETURN_OK;
}

void DiskWriter::writeMaster()
{
  // Master track stuff
  cJSON* masterTrack = cJSON_CreateObject();
  cJSON_AddItemToObject(session, "master", masterTrack );
  GMasterTrack* master = gui->getMasterTrack();
  
  cJSON_AddNumberToObject( masterTrack, "fader", master->getVolume()->value() );
  cJSON_AddNumberToObject( masterTrack, "bpm", gui->getMasterTrack()->getBpm() );
  // TODO add samplerate to session JSON
  //cJSON_AddNumberToObject( masterTrack, "samplerate", gui->getMasterTrack()->getBpm() );
  
  // scene names
  Avtk::ClipSelector* clipSelector = master->getClipSelector();
  cJSON* sceneNames = cJSON_CreateArray();
  cJSON_AddItemToObject( masterTrack, "sceneNames", sceneNames );
  for(int i = 0; i < NSCENES; i++)
  {
    cJSON* sceneName = cJSON_CreateString( clipSelector->clipName(i).c_str() );
    cJSON_AddItemToArray( sceneNames, sceneName );
  }
  
}

int DiskWriter::writeSession()
{
  // add session metadata
  cJSON_AddItemToObject  ( session, "session", cJSON_CreateString( sessionName.c_str() ));
  
  cJSON_AddNumberToObject( session, "version_major", 1 );
  cJSON_AddNumberToObject( session, "version_minor", 0 );
  cJSON_AddNumberToObject( session, "version_patch", 0 );
  
  
  writeMaster();
  
  
  // add JSON "tracks" array
  cJSON* trackArray = cJSON_CreateArray();
  cJSON_AddItemToObject(session, "tracks", trackArray );
  
  // write tracks into JSON tracks array
  for(int t = 0; t < NTRACKS; t++)
  {
    cJSON* track = cJSON_CreateObject();
    cJSON_AddItemToArray( trackArray, track );
    
    // add track metadata: volumes, sends etc
    cJSON_AddNumberToObject( track, "ID", t );
    cJSON_AddStringToObject( track, "name", gui->getTrack(t)->bg.getLabel() );
    
    cJSON_AddNumberToObject( track, "fader", gui->getTrack(t)->getVolume()->value() );
    cJSON_AddNumberToObject( track, "side", gui->getTrack(t)->side.value() );
    cJSON_AddNumberToObject( track, "post", gui->getTrack(t)->post.value() );
    cJSON_AddNumberToObject( track, "reverb", gui->getTrack(t)->rev.value() );
    
    // write clipData vector into clip placeholder
    cJSON* clips = cJSON_CreateArray();
    cJSON_AddItemToObject( track, "clips", clips );
    
    
    
    for(int s = 0; s < NSCENES; s++)
    {
      // add empty string to array
      cJSON* clip = cJSON_CreateString( "" );
      cJSON_AddItemToArray( clips, clip );
      
      // replace blank string if clip exists
      for(unsigned int i = 0; i < clipData.size(); i++)
      {
        if ( clipData.at(i).track == t &&
             clipData.at(i).scene == s )
        {
          cJSON* newClip = cJSON_CreateString( clipData.at(i).name.c_str() );
          cJSON_ReplaceItemInArray( clips, s, newClip );
        }
      }
      
    }
    
  }
  
  
  // write session.luppp JSON node to <path>/<sessionName>.luppp
  stringstream sessionDir;
  sessionDir << sessionPath << "/" << sessionName;
  int sessionDirError = mkdir( sessionDir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  if ( sessionDirError )
  {
    // handle by using different filename?
  }
  
  stringstream sampleDir;
  sampleDir << sessionDir.str() << "/samples";
  int sampleDirError  = mkdir( sampleDir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  
  // FIXME: error check mkdir for error return
  if ( sampleDirError ) {}
  
  stringstream sessionLuppp;
  sessionLuppp << sessionDir.str() << "/session.luppp";
  
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
  
  // clear the clipData, clean page for next save
  clipData.clear();
  
  
  // reset the cJSON objects
  cJSON_Delete( session );
  cJSON_Delete( sample  );
  
  session = cJSON_CreateObject();
  sample  = cJSON_CreateObject();
  
  
  return LUPPP_RETURN_OK;
}
