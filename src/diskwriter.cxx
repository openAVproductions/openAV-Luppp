
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
  sessionJson = cJSON_CreateObject();
  audioJson   = cJSON_CreateObject();
  
  sessionDir = getenv("HOME");
  sessionName = "lupppSession";
  foldersCreated = false;
};

void DiskWriter::initialize(std::string path, std::string name )
{
  sessionName = name;
  
  // write session.luppp JSON node to <path>/<sessionName>.luppp
  stringstream sessionDirStream;
  sessionDirStream << path << "/" << sessionName;
  sessionDir = sessionDirStream.str();
  
  int sessionDirError = mkdir( sessionDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  if ( sessionDirError )
  {
    // handle by using different filename?
    LUPPP_WARN("%s","Error creating session directory");
  }
  
  stringstream audioDirStream;
  audioDirStream << sessionDir << "/audio";
  audioDir = audioDirStream.str();
  LUPPP_NOTE("%s %s","Creating audio dir ", audioDir.c_str() );
  
  int audioDirError  = mkdir( audioDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
  
  // FIXME: error check mkdir for error return
  if ( audioDirError )
  {
    LUPPP_WARN("%s","Error creating sample directory");
  }
  
  foldersCreated = true;
}

std::string DiskWriter::getLastSaveName()
{
  return sessionName;
}

std::string DiskWriter::getLastSavePath()
{
  return sessionDir;
}

int DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab )
{
  if ( !foldersCreated )
  {
    LUPPP_WARN("%s", "Session folders not created yet, while trying to write audioBuffers.");
    return LUPPP_RETURN_ERROR;
  }
  
  // get the filename
  stringstream filename;
  filename << "t_" << track << "_s_" << scene << ".wav";
  
  // store the clip in clipData, we will write the session JSON for it in writeSession
  clipData.push_back( ClipData( track, scene, filename.str() ) );
  
  // add the AudioBuffer metadata to the sample JSON node
  cJSON* sampleClip = cJSON_CreateObject();
  cJSON_AddItemToObject(audioJson, filename.str().c_str(), sampleClip );
  cJSON_AddNumberToObject(sampleClip,"beats", ab->getBeats() );
  
  // write the AudioBuffer contents to <path>/audio/  as  <name>.wav
  // or alternatively t_<track>_s_<scene>.wav
  
  // FIXME: trim trailing  /  sessionPath from session path if its there
  
  stringstream path;
  path << audioDir << "/" << filename.str();
  
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
  cJSON_AddItemToObject(sessionJson, "master", masterTrack );
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
  if ( !foldersCreated )
  {
    LUPPP_WARN("%s", "Session folders not created yet, while trying to write session.");
    return LUPPP_RETURN_ERROR;
  }
  
  // add session metadata
  cJSON_AddItemToObject  ( sessionJson, "session", cJSON_CreateString( sessionName.c_str() ));
  
  cJSON_AddNumberToObject( sessionJson, "version_major", 1 );
  cJSON_AddNumberToObject( sessionJson, "version_minor", 0 );
  cJSON_AddNumberToObject( sessionJson, "version_patch", 0 );
  
  
  writeMaster();
  
  
  // add JSON "tracks" array
  cJSON* trackArray = cJSON_CreateArray();
  cJSON_AddItemToObject(sessionJson, "tracks", trackArray );
  
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
  
  
  
  stringstream sessionLuppp;
  sessionLuppp << sessionDir << "/session.luppp";
  //cout << "Session dir: " << sessionDir.str() << "\n" << "Sample dir : " << audioDir.str() << endl;
  ofstream sessionFile;
  sessionFile.open ( sessionLuppp.str().c_str() );
  sessionFile << cJSON_Print( sessionJson );
  sessionFile.close();
  
  // write the sample JSON node to <samplePath>/sample.cfg
  stringstream audioCfg;
  audioCfg << audioDir << "/audio.cfg";
  
  ofstream audioCfgFile;
  audioCfgFile.open ( audioCfg.str().c_str() );
  audioCfgFile << cJSON_Print( audioJson );
  audioCfgFile.close();
  
  // clear the clipData, clean page for next save
  clipData.clear();
  
  
  // reset the cJSON objects
  cJSON_Delete( sessionJson );
  cJSON_Delete( audioJson   );
  
  sessionJson = cJSON_CreateObject();
  audioJson   = cJSON_CreateObject();
  
  
  return LUPPP_RETURN_OK;
}
