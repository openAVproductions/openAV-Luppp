
#ifndef LUPPP_G_MASTER_TRACK_H
#define LUPPP_G_MASTER_TRACK_H

#include <iostream>
#include <sstream>

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
      
      clipSel(x + 5, y + 26 + 102, 140, 294,""),
      
      volBox(x+5, y+422, 140, 172, ""),
      
      source(x+5, y+26, 140, 100, ""),
      
      tapTempo(x + 25 + 52, y + 26 + 5, 63, 30,"Tap"),
      metronomeButton(x + 10,y + 26 + 5, 63, 30,"Metro"),
      
      volume(x+106, y +427, 36, 150, "Vol")
    {
      ID = privateID++;
      
      tapTempo.callback( gmastertrack_button_callback, &ID );
      metronomeButton.callback( gmastertrack_button_callback, 0 );
      
      for(int i = 0; i < 4; i++)
      {
        beatLights[i] = new Avtk::LightButton( x + 10 + 33 * i, y + 26 + 38, 30, 30, "" );
      }
      
      beatLights[0]->setColor( 0.0, 1.0 , 0.0 );
      beatLights[1]->setColor( 1.0, 1.0 , 0.0 );
      beatLights[2]->setColor( 1.0, 0.48, 0.0 );
      beatLights[3]->setColor( 1.0, 0.0 , 0.0 );
      
      volBox.maximum(1.0f);
      volBox.minimum(0.0f);
      volBox.color( FL_BLACK );
      volBox.selection_color( FL_BLUE );
      
      source.maximum(1.0f);
      source.minimum(0.0f);
      source.color( FL_BLACK );
      source.selection_color( FL_BLUE );
      
      volume.amplitude( 0.75, 0.8 );
      
      end(); // close the group
    }
    
    void setBarBeat(int bar, int beat)
    {
      cout << bar << "  " << beat << endl;
      int num = (beat % 4) + 1;
      
      // turn all off, then on again if its lit
      for( int i = 0; i < 4; i++)
        beatLights[i]->value( 0 );
      for( int i = 0; i < num; i++)
        beatLights[i]->value( 1 );
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
    
    Avtk::Button tapTempo;
    Avtk::LightButton metronomeButton;
    
    Avtk::LightButton* beatLights[4];
    
    Avtk::Volume volume;
    
    static int privateID;
};

#endif // LUPPP_G_MASTER_TRACK_H

