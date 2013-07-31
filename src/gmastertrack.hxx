
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
#include "avtk/avtk_reverb.h"
#include "avtk/avtk_background.h"
#include "avtk/avtk_light_button.h"
#include "avtk/avtk_clip_selector.h"


#include "eventhandler.hxx"

using namespace std;

static void gmastertrack_reverb_cb(Fl_Widget *w, void *data)
{
  int enable = ((Avtk::Reverb*)w)->getActive();
  printf("reverb enable %i\n",enable);
  EventFxReverb e = EventFxReverb( -1, enable, 0.5, 0.5 );
  writeToDspRingbuffer( &e );
}

static void gmastertrack_button_callback(Fl_Widget *w, void *data) {
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
    //Avtk::Button* b = (Avtk::Button*)w;
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
      
      source(x+5, y+26, 140, 100, ""),
      volBox(x+5, y+422, 140, 172, ""),
      
      
      
      tapTempo(x + 25 + 52, y + 26 + 4, 63, 29,"Tap"),
      metronomeButton(x + 9,y + 26 + 4, 64, 29,"Metro"),
      
      sidechain(x+9, y +428, 94, 94, ""),
      reverb   (x+9, y +527, 94, 62, ""),
      
      volume(x+106, y +425, 36, 166, "")
    {
      ID = privateID++;
      
      bar = 0;
      
      tapTempo.callback( gmastertrack_button_callback, &ID );
      metronomeButton.callback( gmastertrack_button_callback, 0 );
      
      reverb.callback( gmastertrack_reverb_cb, 0 );
      
      tapTempo.setBgColor( 0, 0, 0 );
      //metronomeButton.setBgColor( 0, 0, 0 );
      metronomeButton.setColor( 0.4, 0.4, 0.4 );
      //metronomeButton.setOutlineColor( 0.4, 0.4, 0.4 );
      
      for(int i = 0; i < 4; i++)
      {
        int offset = 0;
        /*
        if ( i > 1 )
          offset = 1;
        */
        beatLights[i]   = new Avtk::LightButton( x + 9 + 34 * i + offset, y + 25 + 37     , 30, 29, "" );
        beatLights[i+4] = new Avtk::LightButton( x + 9 + 34 * i + offset, y + 25 + 37 + 32, 30, 29, "" );
      }
      
      beatLights[0]->setColor( 0.0, 1.0 , 0.0 );
      beatLights[1]->setColor( 1.0, 1.0 , 0.0 );
      beatLights[2]->setColor( 1.0, 0.48, 0.0 );
      beatLights[3]->setColor( 1.0, 0.0 , 0.0 );
      
      beatLights[4]->setColor( 0.0, 1.0 , 0.0 );
      beatLights[5]->setColor( 1.0, 1.0 , 0.0 );
      beatLights[6]->setColor( 1.0, 0.48, 0.0 );
      beatLights[7]->setColor( 1.0, 0.0 , 0.0 );
      
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
    
    void setBarBeat(int b, int beat)
    {
      if ( beat % 4 == 0 )
      {
        bar = bar % 4 + 1;
      }
      
      int num = (beat % 4) + 1;
      
      //cout << bar << "  " << num << endl;
      
      // turn all off
      for( int i = 0; i < 8; i++)
        beatLights[i]->value( 0 );
      
      // bar lights
      for( int i = 0; i < bar; i++)
        beatLights[i]->value( 1 );
      
      
      // beat starts at 4
      for( int i = 0; i < num; i++)
        beatLights[i+4]->value( 1 );
      
      
    }
    
    Avtk::Volume* getVolume()
    {
      return &volume;
    }
    Avtk::Reverb* getReverb()
    {
      return &reverb;
    }
    
    ~GMasterTrack()
    {
      free(title);
    }
    
  
  private:
    int ID;
    
    int bar;
    
    char* title;
    
    Avtk::Background bg;
    
    Avtk::ClipSelector clipSel;
    
    Fl_Progress  source;
    Fl_Progress  volBox;
    
    Avtk::Button tapTempo;
    Avtk::LightButton metronomeButton;
    
    Avtk::LightButton* beatLights[8];
    
    Avtk::Reverb sidechain;
    Avtk::Reverb reverb;
    Avtk::Volume volume;
    
    static int privateID;
};

#endif // LUPPP_G_MASTER_TRACK_H

