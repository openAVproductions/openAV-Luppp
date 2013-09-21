
#include "diskreader.hxx"

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <libgen.h>
#include <sys/stat.h>

#include "gui.hxx"
#include "event.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "gmastertrack.hxx"

#include <sndfile.hh>

extern Gui* gui;

using namespace std;

DiskReader::DiskReader()
{
};

void DiskReader::loadSample( int track, int scene, string path )
{
  SndfileHandle infile( path, SFM_READ );
  
  AudioBuffer* ab = new AudioBuffer();
  
  std::vector<float> buf( infile.frames() );
  infile.read( &buf[0] , infile.frames() );
  
  // set the data
  ab->setAudioFrames( infile.frames() );
  ab->nonRtSetSample( buf );
  
  //cout << "DiskReader::loadSample() " << path << " size: " << infile.frames() << endl;
  
  
  if ( infile.frames() > 0 )
  {
    char* basePath = strdup( path.c_str() );
    stringstream base;
    base << dirname( basePath ) << "/sample.cfg";
    
    // open sample, read all
#ifdef DEBUG_STATE
    cout << "loading sample metadata file " << base.str().c_str() << endl;
#endif
    std::ifstream sampleFile( base.str().c_str(), std::ios_base::in|std::ios_base::ate);
    long file_length = sampleFile.tellg();
    if ( file_length > 0 )
    {
      sampleFile.seekg(0, std::ios_base::beg);
      sampleFile.clear();
      char *sampleString = new char[file_length];
      sampleFile.read(sampleString, file_length);
      
      //cout << "Sample file:" << endl << sampleString << endl;
      //cout << "Sample file (parsed):" << endl << cJSON_Parse( sampleString ) << endl;
      
      cJSON* sampleJson = cJSON_Parse( sampleString );
      if (!sampleJson) {
        printf("Error in Sample JSON before: [%s]\n",cJSON_GetErrorPtr());
        return;
      }
      
      // retrieve sample metadata from sample.cfg using filename as key
      char* tmp = strdup( path.c_str() );
      char* baseName = basename( tmp );
      //cout << "tmp " << tmp << " baseName " << baseName << endl;
      
      cJSON* sample = cJSON_GetObjectItem( sampleJson, baseName );
      if ( sample )
      {
        cJSON* beats = cJSON_GetObjectItem( sample, "beats" );
        //cout << "Clip @ " << track << " " << scene << " gets " << beats->valuedouble << " beats."<< endl;
        ab->setBeats( beats->valuedouble );
      }
      else
      {
        cout << "Warning: sample.cfg has no entry for beats." << endl;
      }
      
      cJSON_Delete( sampleJson );
      free ( sampleString  );
      free ( tmp );
    }
    else
    {
#ifdef DEBUG_STATE
      cout << "DiskReader::loadSample() empty or no sample.cfg found" << endl;
#endif
    }
    
    free( basePath );
    
    // write audioBuffer to DSP
    EventLooperLoad e = EventLooperLoad( track, scene, ab );
    writeToDspRingbuffer( &e );
      
    
  }
  
}

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
  

  
  // create cJSON nodes from strings
  session = cJSON_Parse( sessionString );
  if (!session) {
    printf("Error in Session JSON before: [%s]\n",cJSON_GetErrorPtr());
    return;
  }
  
  //cout << "session: " << cJSON_Print( session ) << endl;
  //cout << "sample:  " << cJSON_Print( sample  ) << endl;
  
  
  readTracks();
  
  readMaster();
  
  // cleanup
  cJSON_Delete( session );
  free ( sessionString );
  
}

void DiskReader::readMaster()
{
  cJSON* master = cJSON_GetObjectItem( session, "master");
  if ( master )
  {
    // bpm
    { 
      cJSON* bpm = cJSON_GetObjectItem( master, "bpm");
      LUPPP_NOTE("%s %i","Session: BPM ",bpm->valueint); 
      
      EventTimeBPM e( bpm->valuedouble );
      writeToDspRingbuffer( &e );
    }
    // fader
    { 
      cJSON* fader = cJSON_GetObjectItem( master, "fader");
      EventTrackVol e( -1, fader->valuedouble );
      writeToDspRingbuffer( &e );
    }
    // reverb
    {
      cJSON* reverb = cJSON_GetObjectItem( master, "reverb");
      if ( reverb )
      {
        cJSON* active  = cJSON_GetObjectItem( reverb, "active");
        cJSON* size    = cJSON_GetObjectItem( reverb, "size");
        cJSON* wet     = cJSON_GetObjectItem( reverb, "wet");
        cJSON* damping = cJSON_GetObjectItem( reverb, "damping");
        
        if ( active && size && wet && damping )
        {
          EventFxReverb e(  active->valuedouble, size->valuedouble,
                            wet->valuedouble, damping->valuedouble );
          writeToDspRingbuffer( &e );
        }
      }
      else
      {
#ifdef DEBUG_STATE
        cout << "Session has no reverb element" << endl;
#endif
      }
    }
    // TODO add samplerate to session JSON
    
    // sceneNames
    {
      cJSON* names = cJSON_GetObjectItem( master, "sceneNames");
      if ( names )
      {
        GMasterTrack* master = gui->getMasterTrack();
        Avtk::ClipSelector* clipSelector = master->getClipSelector();
        int nscenes = cJSON_GetArraySize( names );
        for(int s = 0; s < nscenes; s++ )
        {
          cJSON* name = cJSON_GetArrayItem( names, s );
          clipSelector->clipName( s, name->valuestring );
        }
        clipSelector->redraw();
      }
    }
    
    
    
    
  }
  
  
}


void DiskReader::readScenes(int t, cJSON* track)
{
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
        sampleFilePath << sessionPath << "/samples/" << clip->valuestring;
#ifdef DEBUG_STATE
        //LUPPP_NOTE << "clip t " << t << " s " << s << " path " << sampleFilePath.str() << endl;
#endif
        // load it, checking for sample.cfg, and using metadata if there
        loadSample( t, s, sampleFilePath.str() );
      }
    
    } // nClips loop
  
  } 
}

void DiskReader::readTracks()
{
  cJSON* tracks = cJSON_GetObjectItem( session, "tracks");
  if ( tracks )
  {
    int nTracks = cJSON_GetArraySize( tracks );
    for(int t = 0; t < nTracks; t++ )
    {
      cJSON* track = cJSON_GetArrayItem( tracks, t );
      
      readScenes( t, track );
      
      // name
      { 
        cJSON* name = cJSON_GetObjectItem( track, "name");
        gui->getTrack(t)->bg.setLabel( name->valuestring );
      }
      // fader
      { 
        cJSON* fader = cJSON_GetObjectItem( track, "fader");
        EventTrackVol e( t, fader->valuedouble );
        writeToDspRingbuffer( &e );
      }
      // sends
      { 
        cJSON* side = cJSON_GetObjectItem( track, "side");
        cJSON* post = cJSON_GetObjectItem( track, "post");
        cJSON* rev  = cJSON_GetObjectItem( track, "reverb");
        EventTrackSend e1( t, SEND_SIDE, side->valuedouble );
        EventTrackSend e2( t, SEND_POST, post->valuedouble );
        EventTrackSend e3( t, SEND_REV, rev->valuedouble );
        writeToDspRingbuffer( &e1 );
        writeToDspRingbuffer( &e2 );
        writeToDspRingbuffer( &e3 );
      }
      
    } // nTracks loop
    
  }
  else
  {
    cout << "DiskReader: Error getting clip" << endl;
  }
}
