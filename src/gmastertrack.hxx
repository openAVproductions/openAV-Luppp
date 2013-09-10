
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
#include "avtk/avtk_sidechain_gain.h"

#include "avtk/volume.hxx"
#include "avtk/clipselector.hxx"

#include "eventhandler.hxx"

using namespace std;


class GMasterTrack : public Fl_Group
{
  public:
    GMasterTrack(int x, int y, int w, int h, const char* l = 0 );
    
    void setTapTempo( bool b );
    void setBarBeat(int b, int beat);
    
    // FIXME: refactor into time class?
    int bpm;
    
    Avtk::Volume* getInputVolume();
    Avtk::Volume* getVolume();
    Avtk::ClipSelector* getClipSelector();
    
    ~GMasterTrack();
    
  
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
    Avtk::Dial   tempoDial;
    Avtk::Button aboutButton;
    
    Avtk::LightButton* beatLights[4];
    
    Avtk::Volume        inputVolume;
    Avtk::Volume        volume;
    
    static int privateID;
};

#endif // LUPPP_G_MASTER_TRACK_H

