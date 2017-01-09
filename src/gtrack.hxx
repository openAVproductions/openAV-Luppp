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

#ifndef LUPPP_G_TRACK_H
#define LUPPP_G_TRACK_H

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "avtk/avtk_box.h"
#include "avtk/avtk_dial.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_background.h"
#include "avtk/avtk_light_button.h"
#include "avtk/avtk_radial_status.h"

#include "avtk/volume.hxx"
#include "avtk/clipselector.hxx"

#include "config.hxx"
#include "audiobuffer.hxx"
#include "eventhandler.hxx"

using namespace std;

class GTrack : public Fl_Group
{
public:

	Avtk::Volume*       getVolume()
	{
		return &volume;
	}
	Avtk::RadialStatus* getRadialStatus()
	{
		return &radial;
	}
	Avtk::ClipSelector* getClipSelector()
	{
		return &clipSel;
	}

	GTrack(int x, int y, int w, int h, const char* l = 0 );


	// getters
	float getSend();
	float getXSide();
	float getPan() {return panDial.value(); }
	float setPan(float p) {panDial.value( (p+1.f)*0.5 ); }

	bool getSendActive();
	bool getKeyActive();
	bool getRecordActive();

	// setters
	void setSend(float s);
	void setXSide(float s);

	void setSendActive(bool a);
	void setKeyActive(bool a);
	void setRecordActive(bool a);

	void setJackSend(float s);
	void setJackSendActivate(bool a);

	int ID;

	Avtk::Background bg;

	Avtk::RadialStatus radial;

	Avtk::ClipSelector clipSel;

	Avtk::Box    sndBox;
	Avtk::Box    volBox;

	Avtk::Volume volume;

	float getJackSend();
	bool getJackSendActivate();
private:
	Avtk::Box         jackSendBox;
	Avtk::Dial        jackSendDial;
	Avtk::LightButton jackSendActivate;

	Avtk::Dial        sendDial;
	Avtk::LightButton sendActive;

	Avtk::Dial        xsideDial;
	Avtk::LightButton keyActive;

	Avtk::LightButton recordActive;

	Avtk::Dial        panDial;


	static int privateID;
};

#endif // LUPPP_G_TRACK_H

