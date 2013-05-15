
#include "gui.hxx"
#include "avtk/avtk_image.h"

#include <sstream>

// Hack, move to gtrack.cpp
int GTrack::privateID = 0;

using namespace std;

static void gui_button_callback(Fl_Widget *w, void *data)
{
  Avtk::Button* b = (Avtk::Button*)w;
  if ( strcmp( w->label(), "Metronome" ) == 0 )
  {
    b->value( !b->value() );
    EventMetronomeActive e = EventMetronomeActive( b->value() );
    writeToDspRingbuffer( &e );
  }
}

Gui::Gui() :
    window(600,280)
{
  window.color(FL_BLACK);
  window.label("Luppp 5");
  
  Avtk::Image* header = new Avtk::Image(0,0,600,36,"header.png");
  
  metronomeButton = new Avtk::LightButton(0,0,200,30,"Metronome");
  
  for (int i = 0; i < 5; i++ )
  {
    stringstream s;
    s << "Track " << i+1;
    tracks.push_back( new GTrack(8 + i * 118, 40, 110, 230, s.str().c_str() ) );
  }
  
  metronomeButton->callback( gui_button_callback, 0 );
  
  box = new Fl_Box(655, 5, 200, 60, "BPM = 120");
  box->box(FL_UP_BOX);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);
  
  window.end();
}

int Gui::show()
{
  window.show();
  return Fl::run();
}
