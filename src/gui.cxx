
#include "gui.hxx"
#include "avtk/avtk_image.h"

#include <sstream>

#include <FL/Fl_Tooltip.H>

#include "audiobuffer.hxx"

// include the header.c file in the planning dir:
// its the GIMP .c export of the LUPPP header image 
#include "../planning/header.c"

// Hack, move to gtrack.cpp
Fl_Box* Gui::tooltipLabel = 0;
int GTrack::privateID = 0;
int GMasterTrack::privateID = 0;
int AudioBuffer::privateID = 0;

using namespace std;


void close_cb(Fl_Widget*o, void*) {
   if ((Fl::event() == FL_KEYDOWN || Fl::event() == FL_SHORTCUT)
     && Fl::event_key() == FL_Escape)
     return;                            // ignore ESC
   else o->hide();
}

static void gui_static_read_rb(void* inst)
{
  //cout << "read gui" << endl;
  handleGuiEvents();
  Fl::repeat_timeout( 1 / 30.f, &gui_static_read_rb, inst);
}

Gui::Gui() :
    window(1272,750)
{
  fl_show_tooltip = &Gui::show_tooltip;
  
  window.color(FL_BLACK);
  window.label("Luppp 5");
  //window.callback( close_cb, 0 );
  
  
  Avtk::Image* headerImage = new Avtk::Image(0,0,1272,36,"header.png");
  headerImage->setPixbuf( header.pixel_data, 4 );
  
  Gui::tooltipLabel = new Fl_Box(100, 20, 200, 20, "tooltips go here");
  
  //window.resizable( headerImage );
  
  int i = 0;
  for (; i < NTRACKS; i++ )
  {
    stringstream s;
    s << "Track " << i+1;
    tracks.push_back( new GTrack(8 + i * 118, 40, 110, 700, s.str().c_str() ) );
  }
  
  master = new GMasterTrack(8 + i * 118, 40, 150, 700, "Master");
  
  unit = new GUnitTrack(9 + i * 118  + 158, 40, 150, 700, "Units");
  
  
  /*
  box = new Fl_Box(655, 5, 200, 60, "BPM = 120");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);
  */
  window.end();
}

GTrack* Gui::getTrack(int id)
{
  return tracks.at(id);
}

int Gui::show()
{
  window.show();
  
  gui_static_read_rb( this);
  
  return Fl::run();
}

void Gui::show_tooltip( const char* c )
{
  tooltipLabel->label( c );
}
