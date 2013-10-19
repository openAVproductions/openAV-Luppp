
#include "gui.hxx"
#include "avtk/avtk_image.h"
#include "avtk/avtk_button.h"

#include <sstream>

// ONLY to be used for QUIT!
#include "jack.hxx"
extern Jack* jack;

#include "audiobuffer.hxx"
#include "controller/genericmidi.hxx"


#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Wizard.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Multiline_Output.H>

// include the header.c file in the planning dir:
// its the GIMP .c export of the LUPPP header image 
#include "../planning/header.c"

// Hack, move to gtrack.cpp
int GTrack::privateID = 0;
int GMasterTrack::privateID = 0;
//int AudioBuffer::privateID = 0;

using namespace std;

extern Gui* gui;
extern int signalHanlderInt;

static void signalChecker(void*)
{
  if ( signalHanlderInt )
  {
    // Luppp recieved either a SIGTERM or SIGINT: quit gracefully
    gui->quit();
  }
  else
  {
    Fl::repeat_timeout( 0.1, (Fl_Timeout_Handler)&signalChecker, 0 );
  }
}

void close_cb(Fl_Widget*o, void*)
{
  if ( (Fl::event() == FL_KEYDOWN || Fl::event() == FL_SHORTCUT) && Fl::event_key() == FL_Escape)
  {
    // on excape, as about quitting
    gui->askQuit();
  }
  else
  {
    gui->quit();
  }
}
static void gui_static_read_rb(void* inst)
{
  handleGuiEvents();
  Fl::repeat_timeout( 1 / 30.f, &gui_static_read_rb, inst);
}

void option_controller_cb(Fl_Widget*,void* data)
{
  LUPPP_NOTE("%s","Controller cb");
}

static void gui_header_callback(Fl_Widget *w, void *data)
{
  Gui* g = (Gui*)data;
  if ( Fl::event_x() > 130 )
  {
    return;
  }
  
  Fl_Menu_Item rclick_menu[] =
  {
    { "New Session" },
    { "Load Session" },
    { "Save Session   ", 0, 0, 0, FL_MENU_DIVIDER},
    { "Setup", 0, 0, 0, FL_MENU_DIVIDER},
    { "Quit" },
    { 0 }
  };
  
  Fl_Menu_Item *m = (Fl_Menu_Item*) rclick_menu->popup( 10, 38, 0, 0, 0);
  
  if ( !m )
  {
      return;
  }
  else if ( strcmp(m->label(), "New Session") == 0 )
  {
    int yes = fl_choice("Start a new session?","Cancel","Yes",0);
    if ( yes )
    {
      gui->reset();
    }
  }
  else if ( strcmp(m->label(), "Load Session") == 0 )
  {
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Load Session");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
    fnfc.directory( getenv("HOME") );
    
    switch ( fnfc.show() )
    {
      case -1: //printf("ERROR: %s\\n", fnfc.errmsg());
          break;  // ERROR
      case  1: //printf("CANCEL\\n");
          break;  // CANCEL
      default: printf("Loading session directory %s\n", fnfc.filename());
          gui->getDiskReader()->readSession( fnfc.filename() );
          break;
    }
  }
  else if ( strcmp(m->label(), "Save Session   ") == 0 )
  {
    const char* name = fl_input( "Save session as", gui->getDiskWriter()->getLastSaveName().c_str() );
    if ( name )
    {
      gui->getDiskWriter()->initialize( getenv("HOME"), name );
      LUPPP_NOTE("%s %s","Saving session as ", name );
      EventStateSave e;
      writeToDspRingbuffer( &e );
    }
  }
  else if ( strcmp(m->label(), "Setup") == 0 )
  {
    g->getOptionsWindow()->show();
  }
  else if ( strcmp(m->label(), "Quit") == 0 )
  {
    g->askQuit();
  }
}

OptionsWindow* Gui::getOptionsWindow()
{
  return optionWindow;
}

void Gui::selectLoadController(Fl_Widget* w, void*)
{
  // FIXME: refactor
  string path;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a controller definition");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fnfc.filter("Controllers\t*.ctlr");
  fnfc.directory( getenv("HOME") ); // default directory to use
  // Show native chooser
  switch ( fnfc.show() ) {
     case -1: printf("ERROR: %s\n", fnfc.errmsg());    break;  // ERROR
     case  1: printf("CANCEL\n");                      break;  // CANCEL
     default: printf("Loading controller at %s\n", fnfc.filename());
        path = fnfc.filename();
        break;
  }
  
  if ( strcmp( path.c_str(), "" ) == 0 )
    return;
  
  LUPPP_NOTE("%s","ADD Controller cb");
  Controller* c = new GenericMIDI( path );
  
  if ( c->status() == Controller::CONTROLLER_OK )
  {
    EventControllerInstance e(c);
    writeToDspRingbuffer( &e );
  }
  else
  {
    LUPPP_ERROR("Controller initialization failed!");
  }
  
}

void Gui::selectLoadSample( int track, int scene )
{
  // FIXME: refactor
  string path;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a file");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fnfc.filter("Wav\t*.wav");
  fnfc.directory( getenv("HOME") ); // default directory to use
  // Show native chooser
  switch ( fnfc.show() ) {
     case -1: printf("ERROR: %s\n", fnfc.errmsg());    break;  // ERROR
     case  1: printf("CANCEL\n");                      break;  // CANCEL
     default: printf("Loading directory: %s\n", fnfc.filename());
        // update path and load it
        path = fnfc.filename();
        break;
  }
  
  if ( strcmp( path.c_str(), "" ) == 0 )
    return;
  
  // diskReader loads sample, and parses for sample.cfg
  diskReader->loadSample( track, scene, path );
}

AudioEditor* Gui::getAudioEditor()
{
  return audioEditor;
}

Gui::Gui() :
    samplerate( 0 ),
    window(1110,650),
    
    diskReader( new DiskReader() ),
    diskWriter( new DiskWriter() )
{
  LUPPP_NOTE( "%s", "Gui()" );
  
  // setup callback to signalChecker()
  Fl::add_timeout( 0.1, (Fl_Timeout_Handler)&signalChecker, 0 );
  
  window.color(FL_BLACK);
  window.label("Luppp");
  window.callback( close_cb, 0 );
  
  Avtk::Image* headerImage = new Avtk::Image(0,0,1110,36,"header.png");
  headerImage->setPixbuf( header.pixel_data, 4 );
  headerImage->callback( gui_header_callback, this );
  
  /*
  tooltipLabel = new Fl_2(130, 25, 500, 20, "");
  tooltipLabel->labelcolor( FL_LIGHT2 );
  tooltipLabel->color( FL_DARK2 );
  tooltipLabel->hide();
  //tooltipLabel->align( FL_ALIGN_TOP_LEFT );
  */
  
  window.resizable( headerImage );
  
  int i = 0;
  for (; i < NTRACKS; i++ )
  {
    stringstream s;
    s << "Track " << i+1;
    //printf("track name %s\n", s.str().c_str() );
    tracks.push_back( new GTrack(8 + i * 118, 40, 110, 600, s.str().c_str() ) );
  }
  
  master = new GMasterTrack(8 + i * 118, 40, 150, 600, "Master");
  
  window.end();
  
  optionWindow = new OptionsWindow();
  
  // Create AudioEditor after window.end() has been called
  audioEditor = new AudioEditor();
  
  // default controller for testing
  LUPPP_NOTE("Adding APC40 Controller cb");
  //GenericMIDI* c = new GenericMIDI( "akai_apc.ctlr" );
  GenericMIDI* c = new GenericMIDI();
  
  std::vector<Binding*> b = c->getMidiToAction();
  for(unsigned int i =0; i < b.size(); i++ )
    optionWindow->addBinding( b.at(i) );
  
  
  if ( c->status() == Controller::CONTROLLER_OK )
  {
    EventControllerInstance e(c);
    writeToDspRingbuffer( &e );
  }
  else
  {
    LUPPP_ERROR("Controller initialization failed!");
  }
  
  
}

void Gui::reset()
{
  // signal to DSP to reset state
  EventStateReset ev;
  writeToDspRingbuffer( &ev );
  
  // clear UI state: track names / scene names
  for(unsigned int i = 0; i < NTRACKS; i++)
  {
    stringstream s;
    s << "Track " << i+1;
    tracks.at(i)->bg.setLabel( s.str().c_str() );
    
    for(unsigned int s = 0; s < NSCENES; s++)
    {
      tracks.at(i)->getClipSelector()->clipName( s, "" );
    }
  }
  
  for(unsigned int i = 0; i < NSCENES; i++)
  {
    stringstream s;
    s << "Scene " << i+1;
    master->getClipSelector()->clipName( i, s.str() );
  }
  
}

GTrack* Gui::getTrack(int id)
{
  return tracks.at(id);
}

int Gui::show()
{
  window.show();
  
  gui_static_read_rb( this );
  
  return Fl::run();
}


int Gui::quit()
{
  jack->quit();
  window.hide();
  exit(0);
}

void Gui::askQuit()
{
  int quit = fl_choice("Really Quit?","Cancel","Quit",0);
  if ( quit ) // JUST QUIT
  {
    gui->quit();
  }
}


Gui::~Gui()
{
  delete optionWindow;
  delete audioEditor;
  
  delete diskReader;
  delete diskWriter;
  
  delete master;
  
  for(unsigned int i = 0; i < tracks.size(); i++)
  {
    delete tracks.at(i);
  }
  
  
}
