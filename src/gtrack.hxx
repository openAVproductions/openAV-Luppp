
#ifndef LUPPP_G_TRACK_H
#define LUPPP_G_TRACK_H

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "avtk/avtk_dial.h"
#include "avtk/avtk_volume.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_background.h"
#include "avtk/avtk_clip_selector.h"


#include "config.hxx"
#include "worker.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"

using namespace std;

static void gtrack_reverb_cb(Fl_Widget *w, void *data);
extern void gtrack_button_callback(Fl_Widget *w, void *data);

class GTrack : public Fl_Group
{
  public:
    
    Avtk::Volume*       getVolume(){return &volume;}
    Avtk::ClipSelector* getClipSelector(){return &clipSel;}
    
    GTrack(int x, int y, int w, int h, const char* l = 0 ) :
      Fl_Group(x, y, w, h),
      title( strdup(l) ),
      bg( x, y , w, h, title ),
      
      clipSel(x + 5, y + 26 + 102, 100, 294,""),
      
      volBox(x+5, y+422, 100, 172, ""),
      
      volume(x+65, y +427, 36, 150, "Vol"),
      
      
      side(x+22, y +440 +  0, 30, 30, "S-C"),
      post(x+22, y +440 + 50, 30, 30, "P-S"),
      rev (x+22, y +440 +100, 30, 30, "Rev"),
      
      progress(x+5, y+ 26, 100, 100, "Source UI")
    {
      ID = privateID++;
      
      clipSel.setID( ID );
      
      rev.callback( gtrack_reverb_cb, this );
      
      volume.callback( gtrack_button_callback, 0 );
      
      progress.maximum(1.0f);
      progress.minimum(0.0f);
      progress.color( FL_BLACK );
      progress.selection_color( FL_BLUE );
      
      volBox.maximum(1.0f);
      volBox.minimum(0.0f);
      volBox.color( FL_BLACK );
      volBox.selection_color( FL_BLUE );
      
      end(); // close the group
    }
    
    ~GTrack()
    {
      free(title);
    }
    
    int ID;
    
    char* title;
    
    Avtk::Background bg;
    
    Avtk::ClipSelector clipSel;
    
    Fl_Progress  volBox;
    /*
    Avtk::Button button1;
    Avtk::Button button2;
    Avtk::Button button3;
    Avtk::Button button4;
    Avtk::Button button5;
    Avtk::Button button6;
    */
    
    Avtk::Volume volume;
    
    Avtk::Dial   side;
    Avtk::Dial   post;
    Avtk::Dial   rev;
    
    Fl_Progress  progress;
    
    static int privateID;
};

void gtrack_reverb_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_REV, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i reverb send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}

#endif // LUPPP_G_TRACK_H

