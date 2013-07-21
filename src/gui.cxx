
#include "gui.hxx"
#include "avtk/avtk_image.h"

#include <sstream>

#include "audiobuffer.hxx"

// Hack, move to gtrack.cpp
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
    window(1200,750)
{
  window.color(FL_BLACK);
  window.label("Luppp 5");
  //window.callback( close_cb, 0 );
  
  Avtk::Image* header = new Avtk::Image(0,0,1200,36,"header.png");
  
  
  int i = 0;
  for (; i < NTRACKS; i++ )
  {
    stringstream s;
    s << "Track " << i+1;
    tracks.push_back( new GTrack(8 + i * 118, 40, 110, 650, s.str().c_str() ) );
  }
  
  master = new GMasterTrack(9 + i * 118, 40, 150, 250, "Master");
  
  
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
