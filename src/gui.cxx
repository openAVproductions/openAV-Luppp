
#include "gui.hxx"
#include "avtk/avtk_image.h"

#include <sstream>

#include "jack.hxx"
#include "audiobuffer.hxx"

#include <FL/fl_ask.H>

// include the header.c file in the planning dir:
// its the GIMP .c export of the LUPPP header image 
#include "../planning/header.c"

// Hack, move to gtrack.cpp
int GTrack::privateID = 0;
int GMasterTrack::privateID = 0;
//int AudioBuffer::privateID = 0;

using namespace std;

extern Gui* gui;

void close_cb(Fl_Widget*o, void*) {
   if ((Fl::event() == FL_KEYDOWN || Fl::event() == FL_SHORTCUT)
     && Fl::event_key() == FL_Escape)
     return; // ignore ESC
   else o->hide();
}

static void gui_static_read_rb(void* inst)
{
  //cout << "read gui" << endl;
  handleGuiEvents();
  Fl::repeat_timeout( 1 / 30.f, &gui_static_read_rb, inst);
}

static void gui_header_callback(Fl_Widget *w, void *data)
{
  if ( Fl::event_x() > 130 )
  {
    return;
  }
  
  Fl_Menu_Item rclick_menu[] =
  {
    { "New Session" },
    { "Load Session" },
    { "Save Session   ", 0, 0, 0, FL_MENU_DIVIDER},
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
      EventStateReset ev;
      writeToDspRingbuffer( &ev );
    }
  }
  else if ( strcmp(m->label(), "Load Session") == 0 )
  {
    cout << "Load clicked" << endl;
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
      cout << "Save clicked, name = " << name << endl;
      gui->getDiskWriter()->initialize( getenv("HOME"), name );
      EventStateSave e;
      writeToDspRingbuffer( &e );
    }
  }
  else if ( strcmp(m->label(), "Quit") == 0 )
  {
    int choice = fl_choice("Really Quit?","Cancel","Just Quit","Save & Quit",0);
    //cout << choice << endl;
    if ( choice == 2 ) // SAVE & QUIT
    {
      const char* name = fl_input( "Save session as", gui->getDiskWriter()->getLastSaveName().c_str() );
      if ( name )
      {
        cout << "Save clicked, name = " << name << endl;
        gui->getDiskWriter()->initialize( getenv("HOME"), name );
        EventStateSave e;
        writeToDspRingbuffer( &e );
      }
      else
      {
        return;
      }
    }
    else if ( choice == 1 ) // JUST QUIT
    {
      EventQuit e;
      writeToDspRingbuffer( &e );
    }
    else
    {
      return;
    }
  }
}

void Gui::selectLoadSample( int track, int scene )
{
  // FIXME: refactor
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
        break;
  }
  
  if ( strcmp( path.c_str(), "" ) == 0 )
    return;
  
  // diskReader loads sample, and parses for sample.cfg
  diskReader->loadSample( track, scene, path );
}


Gui::Gui() :
    window(1110,650),
    diskReader( new DiskReader() ),
    diskWriter( new DiskWriter() )
{
  LUPPP_NOTE( "%s", "Gui()" );
  window.color(FL_BLACK);
  window.label("Luppp");
  //window.callback( close_cb, 0 );
  
  Avtk::Image* headerImage = new Avtk::Image(0,0,1110,36,"header.png");
  headerImage->setPixbuf( header.pixel_data, 4 );
  headerImage->callback( gui_header_callback, this );
  
  tooltipLabel = new Fl_Box(130, 25, 500, 20, "");
  tooltipLabel->labelcolor( FL_LIGHT2 );
  tooltipLabel->color( FL_DARK2 );
  tooltipLabel->hide();
  //tooltipLabel->align( FL_ALIGN_TOP_LEFT );
  
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

