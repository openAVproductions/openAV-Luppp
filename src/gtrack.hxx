
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
      
      volBox(x+5, y+522+2, 100, 172, ""),
      
      /*
      button1(x + 5, y + 324 + 102, 100, 18,"Rec"),
      button2(x + 5, y + 344 + 102, 100, 18,"Play"),
      button3(x + 5, y + 364 + 102, 100, 18,"Stop"),
      button4(x + 5, y + 384 + 102,  48, 18,"-"),
      button5(x +57, y + 384 + 102,  48, 18,"+"),
      button6(x + 5, y + 404 + 102, 100, 18,"Load"),
      */
      
      volume(x+66, y +527, 36, 150, "Vol"),
      
      dial1(x+22, y +440 + 100, 30, 30, "Rev"),
      dial2(x+22, y +440 + 150, 30, 30, "S-C"),
      dial3(x+22, y +440 + 200, 30, 30, "P-S"),
      
      progress(x+5, y+ 26, 100, 100, "Source UI"),
      
      //unit(x+5, y+200+26, 100, 100, "Unit"),
      
      fx(x+5, y+ 421, 100, 100, "FX")
    {
      ID = privateID++;
      /*
      button1.callback( gtrack_button_callback, &ID );
      button2.callback( gtrack_button_callback, &ID );
      button3.callback( gtrack_button_callback, &ID );
      button4.callback( gtrack_button_callback, &ID );
      button5.callback( gtrack_button_callback, &ID );
      button6.callback( gtrack_button_callback, &ID );
      */
      volume.callback( gtrack_button_callback, 0 );
      
      volume.amplitude( 0.75, 0.5 );
      
      progress.maximum(1.0f);
      progress.minimum(0.0f);
      progress.color( FL_BLACK );
      progress.selection_color( FL_BLUE );
      
      fx.maximum(1.0f);
      fx.minimum(0.0f);
      fx.color( FL_BLACK );
      fx.selection_color( FL_BLUE );
      
      
      volBox.maximum(1.0f);
      volBox.minimum(0.0f);
      volBox.color( FL_BLACK );
      volBox.selection_color( FL_BLUE );
      /*
      unit.maximum(1.0f);
      unit.minimum(0.0f);
      unit.color( FL_BLACK );
      unit.selection_color( FL_BLUE );
      */
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
    //Fl_Progress       unit;
    Fl_Progress       fx;
    
    
    static int privateID;
};

#endif // LUPPP_G_TRACK_H

