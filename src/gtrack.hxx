
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

extern void gtrack_button_callback(Fl_Widget *w, void *data);

class GTrack : public Fl_Group
{
  public:
    
    Avtk::ClipSelector* getClipSelector(){return &clipSel;}
    
    GTrack(int x, int y, int w, int h, const char* l = 0 ) :
      Fl_Group(x, y, w, h),
      title( strdup(l) ),
      bg( x, y , w, h, title ),
      
      clipSel(x + 5, y + 26 + 102, 100, 294,""),
      
      volBox(x+5, y+422, 100, 172, ""),
      
      volume(x+65, y +427, 36, 150, "Vol"),
      
      dial1(x+22, y +440     , 30, 30, "Rev"),
      dial2(x+22, y +440 + 50, 30, 30, "S-C"),
      dial3(x+22, y +440 +100, 30, 30, "P-S"),
      
      progress(x+5, y+ 26, 100, 100, "Source UI")
    {
      ID = privateID++;
      
      volume.callback( gtrack_button_callback, 0 );
      
      volume.amplitude( 0.75, 0.5 );
      
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
    
    Avtk::Dial   dial1;
    Avtk::Dial   dial2;
    Avtk::Dial   dial3;
    
    Fl_Progress  progress;
    
    static int privateID;
};

#endif // LUPPP_G_TRACK_H

