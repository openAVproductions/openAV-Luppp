
#ifndef LUPPP_G_MASTER_TRACK_H
#define LUPPP_G_MASTER_TRACK_H

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Progress.H>

#include "avtk/avtk_dial.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_background.h"
#include "avtk/avtk_light_button.h"
#include "avtk/avtk_clip_selector.h"


#include "eventhandler.hxx"

using namespace std;

static void gmastertrack_button_callback(Fl_Widget *w, void *data) {
  
  
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

class GMasterTrack : public Fl_Group
{
  public:
    GMasterTrack(int x, int y, int w, int h, const char* l = 0 ) :
      Fl_Group(x, y, w, h),
      title( strdup(l) ),
      bg( x, y , w, h, title ),
      
      clipSel(x + 5, y + 26 + 102, 140, 294,"Testing"),
      
      volBox(x+5, y+522+2, 140, 172, ""),
      
      source(x+5, y+26, 140, 100, "Audio IN"),
      fxBox(x+5, y+ 421, 140, 100, ""),
      
      tapTempo(x + 25 + 52, y + 450 + 26, 66, 30,"Tap"),
      metronomeButton(x + 7,y + 450 + 26, 66, 30,"Metro"),
      
      dial1(x+54, y + 420, 44, 44, "BPM"),
      
      volume(x+108, y +530, 36, 150, "Vol")
    {
      ID = privateID++;
      
      tapTempo.callback( gmastertrack_button_callback, &ID );
      
      volume.amplitude( 0.75, 0.8 );
      
      volBox.maximum(1.0f);
      volBox.minimum(0.0f);
      volBox.color( FL_BLACK );
      volBox.selection_color( FL_BLUE );
      fxBox.maximum(1.0f);
      fxBox.minimum(0.0f);
      fxBox.color( FL_BLACK );
      fxBox.selection_color( FL_BLUE );
      source.maximum(1.0f);
      source.minimum(0.0f);
      source.color( FL_BLACK );
      source.selection_color( FL_BLUE );
      
      metronomeButton.callback( gmastertrack_button_callback, 0 );
      dial1.callback( gmastertrack_button_callback, 0 );
      
      end(); // close the group
    }
    
    ~GMasterTrack()
    {
      free(title);
    }
    
  
  private:
    int ID;
    
    char* title;
    
    Avtk::Background bg;
    
    Avtk::ClipSelector clipSel;
    
    Fl_Progress  source;
    Fl_Progress  volBox;
    Fl_Progress  fxBox;
    
    Avtk::Button tapTempo;
    Avtk::LightButton metronomeButton;
    
    Avtk::Dial dial1;
    
    Avtk::Volume volume;
    
    static int privateID;
};

#endif // LUPPP_G_MASTER_TRACK_H

