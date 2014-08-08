/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LUPPP_G_MASTER_TRACK_H
#define LUPPP_G_MASTER_TRACK_H

#include <iostream>
#include <sstream>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Progress.H>

#include "avtk/avtk_box.h"
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
    
    int  getBpm();
    void setBpm( int bpm );
    void setTapTempo( bool b );
    void setBarBeat(int b, int beat);
    
    void setReturnVol(float f);
    void setInputVol(float f);
    void setInputTo(int to, float f);
    void setInputToActive(int to, bool f);
    
    void metronomeEnable( bool b );
    
    Avtk::Volume* getInputVolume();
    Avtk::Volume* getVolume();
    Avtk::ClipSelector* getClipSelector();
    
    Avtk::LightButton* getInputToSend();
    Avtk::LightButton* getInputToSidechainKey();
    Avtk::LightButton* getInputToMix();
    
    Avtk::Dial*        getInputToSendVol();
    Avtk::Dial*        getInputToXSide();
    Avtk::Dial*        getInputToMixVol();
    
    ~GMasterTrack();
  
  private:
    int ID;
    char* title;
    
    int bar;
    int bpm;
    
    Avtk::Background bg;
    
    Avtk::ClipSelector clipSel;
    
    Avtk::Box  source;
    Avtk::Box  volBox;
    
    Avtk::Button transport;
    Avtk::Button tapTempo;
    Avtk::LightButton metronomeButton;
    Avtk::Dial   tempoDial;
    
    Avtk::Dial   returnVol;
    
    Avtk::LightButton* beatLights[4];
    
    Avtk::Volume        inputVolume;
    
    Avtk::LightButton   inputToSend;
    Avtk::Dial          inputToSendVol;
    
    Avtk::LightButton   inputToSidechainKey;
    Avtk::Dial          inputToSidechainSignalVol;
    
    Avtk::LightButton   inputToMix;
    Avtk::Dial          inputToMixVol;
    
    Avtk::Volume        volume;
    
    static int privateID;
};

#endif // LUPPP_G_MASTER_TRACK_H

