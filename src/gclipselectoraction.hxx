
#ifndef LUPPP_G_CLIP_SELECTOR_ACTION_H
#define LUPPP_G_CLIP_SELECTOR_ACTION_H

#include "config.hxx"
#include "worker.hxx"
#include "looper.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"

static string choose_file()
{
  string path;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  //fnfc.filter("Wav\t*.wav");
  fnfc.directory( getenv("HOME") ); // default directory to use
  // Show native chooser
  switch ( fnfc.show() ) {
   case -1: printf("ERROR: %s\n", fnfc.errmsg());    break;  // ERROR
   case  1: printf("CANCEL\n");                      break;  // CANCEL
   default: printf("Loading directory: %s\n", fnfc.filename());    
    
    // update path and load it
    path = fnfc.filename();
    
    break;  // FILE CHOSEN
  }
  return path;
}

static int clipSelectorLoad(int track, int scene)
{
  string filePathName = choose_file();
  
  if ( strcmp( filePathName.c_str(), "" ) == 0 )
    return -1;
  
  AudioBuffer* ab = Worker::loadSample( filePathName );
  EventLooperLoad e = EventLooperLoad( track, scene, ab );
  writeToDspRingbuffer( &e );
  
  return 0;
}

#endif // LUPPP_G_CLIP_SELECTOR_ACTION_H

