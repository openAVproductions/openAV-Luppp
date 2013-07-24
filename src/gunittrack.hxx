
#ifndef LUPPP_G_UNIT_TRACK_H
#define LUPPP_G_UNIT_TRACK_H

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>

#include "avtk/avtk_unit.h"
#include "avtk/avtk_dial.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_background.h"


#include "eventhandler.hxx"

using namespace std;

static void gunittrack_button_callback(Fl_Widget *w, void *data) {
  
  
  int track = 0;
  if ( data )
   track = *(int*)data;
  
  if ( strcmp( w->label(), "Metro" ) == 0 )
  {
    Avtk::Button* b = (Avtk::Button*)w;
    b->value( !b->value() );
    EventMetronomeActive e = EventMetronomeActive( b->value() );
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label(), "BPM" ) == 0 )
  {
    Avtk::Dial* b = (Avtk::Dial*)w;
    float bpm = b->value() * 160 + 60; // 60 - 220
    EventTimeBPM e = EventTimeBPM( bpm );
    writeToDspRingbuffer( &e );
  }
  else if ( strcmp( w->label(), "Tap" ) == 0 )
  {
    Avtk::Button* b = (Avtk::Button*)w;
    EventTimeTempoTap e;
    writeToDspRingbuffer( &e );
  }
  else
  {
    cout << __FILE__ << __LINE__ << " Error: unknown command string" << endl;
  }
}

class GUnitTrack : public Fl_Group
{
  public:
    GUnitTrack(int x, int y, int w, int h, const char* l = 0 ) :
      Fl_Group(x, y, w, h),
      title( strdup(l) ),
      bg( x, y , w, h, title )
    {
      int uh = h / 5;
      unit[0] = new Avtk::Unit(x + 2, y + uh * 4 - 2, w - 6, h / 5 - 2,"1");
      unit[1] = new Avtk::Unit(x + 2, y + uh * 3 - 2, w - 6, h / 5 - 2,"2");
      unit[2] = new Avtk::Unit(x + 2, y + uh * 2 - 2, w - 6, h / 5 - 2,"3");
      unit[3] = new Avtk::Unit(x + 2, y + uh * 1 - 2, w - 6, h / 5 - 2,"4");
      unit[4] = new Avtk::Unit(x + 2, y - 2         , w - 6, h / 5 - 2,"5");
      
      end(); // close the group
    }
    
    ~GUnitTrack()
    {
      free(title);
    }
    
  
  private:
    char* title;
    
    Avtk::Background bg;
    
    Avtk::Unit*      unit[5];
    
};

#endif // LUPPP_G_UNIT_TRACK_H

