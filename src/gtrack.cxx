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

#include "gtrack.hxx"
#include "gui.hxx"

int GTrack::privateID = 0;
int GMasterTrack::privateID = 0;

static void gtrack_vol_cb(Fl_Widget *w, void *data);
static void gtrack_pan_cb(Fl_Widget *w, void *data);
static void gtrack_key_cb(Fl_Widget *w, void *data);
static void gtrack_xsideDial_cb(Fl_Widget *w, void *data);
static void gtrack_sendDial_cb(Fl_Widget *w, void *data);
static void gtrack_send_cb(Fl_Widget *w, void *data);
static void gtrack_jacksend_cb(Fl_Widget *w, void *data);
static void gtrack_record_cb(Fl_Widget *w, void *data);
static void gtrack_jacksendactivate_cb(Fl_Widget* w,void *data);


GTrack::GTrack(int x, int y, int w, int h, const char* l ) :
	Fl_Group(x, y, w, h),
	bg( x, y , w, h, l ),

	radial( x+5, y+ 26, 100, 100, ""),

	clipSel(x + 5, y + 26 + 102, 100, 294,""),
	jackSendBox(x+5, y+422, 100, 45, ""),
	jackSendDial(x+21, y+422+8, 30,30, ""),
	jackSendActivate(x+66, y+422+11, 36,25, "FX"),
	sndBox(x+5   , y+422+50, 58, 172, ""),
	volBox(x+5+62, y+422+50, 40, 172, ""),
	panDial(x+66+7, y +425+54, 30, 30, ""),
	volume(x+66+4, y +425+50+38, 32, 166-38, ""),

	sendDial   (x+19, y +430 +  50, 30, 30, ""),
	sendActive (x+ 9, y +430 +  82, 50, 25, "Snd"),

	xsideDial    (x+19, y +430 + 69+50, 30, 30, ""),
	keyActive  (x+ 9, y +430 + 151, 50, 25, "Key"),

	recordActive  (x+ 9, y +430 + 182, 50, 25, "XRec")
{
	ID = privateID++;

	clipSel.setID( ID );

	sendDial.callback( gtrack_sendDial_cb, this );
	sendActive.setColor( 0, 1.0, 0.0 );
	sendActive.callback( gtrack_send_cb, this );

	xsideDial.callback( gtrack_xsideDial_cb, this );
	xsideDial.align( FL_ALIGN_BOTTOM );
	keyActive.callback( gtrack_key_cb, this );
	keyActive.setColor( 0, 0.6, 1 );

	recordActive.setColor( 1, 0.0, 0.0 );
	recordActive.callback( gtrack_record_cb, this );

	volume.callback( gtrack_vol_cb, this );
	panDial.callback( gtrack_pan_cb, this );
	panDial.value( 0.5f );

	jackSendActivate.setColor( 1, 1, 0 );
	jackSendActivate.callback(gtrack_jacksendactivate_cb,this);
	jackSendDial.align(FL_ALIGN_INSIDE);
	jackSendDial.callback(gtrack_jacksend_cb,this);
	jackSendDial.value(1.0);
	//volBox.color( fl_rgb_color( 0,0,0 ) );

	end(); // close the group
}

float GTrack::getSend()
{
	return sendDial.value();
}
float GTrack::getXSide()
{
	return xsideDial.value();
}

bool GTrack::getSendActive  ()
{
	return sendActive.value();
}
bool GTrack::getKeyActive   ()
{
	return keyActive.value();
}
bool GTrack::getRecordActive()
{
	return recordActive.value();
}


void GTrack::setSend(float s)
{
	sendDial.value( s );
}
void GTrack::setXSide(float s)
{
	xsideDial.value( s );
}

void GTrack::setSendActive(bool a)
{
	sendActive.value( a );
}
void GTrack::setKeyActive(bool a)
{
	keyActive.value( a );
}
void GTrack::setRecordActive(bool a)
{
	recordActive.value( a );
}

void GTrack::setJackSend(float s)
{
	jackSendDial.value(s);
}

void GTrack::setJackSendActivate(bool a)
{
	jackSendActivate.value(a);
}

float GTrack::getJackSend()
{
	return jackSendDial.value();
}

bool GTrack::getJackSendActivate()
{
	return jackSendActivate.value();
}

void gtrack_sendDial_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	EventTrackSend e( track->ID, SEND_POSTFADER, ((Avtk::Dial*)w)->value() );
	writeToDspRingbuffer( &e );
	//printf("track %i reverb send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_key_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	Avtk::LightButton* d = (Avtk::LightButton*)w;
	bool b = d->value();
	if ( b < 0.5 ) {
		EventTrackSendActive e( track->ID, SEND_KEY, true );
		writeToDspRingbuffer( &e );
	} else {
		EventTrackSendActive e( track->ID, SEND_KEY, false );
		writeToDspRingbuffer( &e );
	}
	//printf("track %i post send %s\n", track->ID, b ? "off" : "on" );
}


void gtrack_xsideDial_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	EventTrackSend e( track->ID, SEND_XSIDE, ((Avtk::Dial*)w)->value() );
	writeToDspRingbuffer( &e );
	//printf("track %i side send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}

void gtrack_vol_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	EventTrackVol e( track->ID, ((Avtk::Volume*)w)->value() );
	writeToDspRingbuffer( &e );
	//printf("track %i vol %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}

void gtrack_pan_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	/* -1 to 1 range, 0 is center */
	EventTrackPan e( track->ID, (((Avtk::Dial*)w)->value() * 2) - 1 );
	writeToDspRingbuffer( &e );
}

void gtrack_send_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	Avtk::LightButton* d = (Avtk::LightButton*)w;
	bool b = d->value();
	d->value( !b );
	if ( b < 0.5 ) {
		EventTrackSendActive e( track->ID, SEND_POSTFADER, 1.0f );
		writeToDspRingbuffer( &e );
	} else {
		EventTrackSendActive e( track->ID, SEND_POSTFADER, 0.0f );
		writeToDspRingbuffer( &e );
	}
	//printf("track %i reverb send %s\n", track->ID, b ? "true" : "false" );
}
void gtrack_jacksend_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	Avtk::Dial* d = (Avtk::Dial*)w;
	float v = d->value();
	EventTrackJackSend ev(track->ID,v);
	writeToDspRingbuffer(&ev);

	//printf("track %i reverb send %s\n", track->ID, b ? "true" : "false" );
}
void gtrack_record_cb(Fl_Widget *w, void *data)
{
	GTrack* track = (GTrack*) data;
	Avtk::LightButton* d = (Avtk::LightButton*)w;
	bool b = d->value();
	if ( b < 0.5 ) {
		EventTrackRecordArm e( track->ID, 1.0f );
		writeToDspRingbuffer( &e );
	} else {
		EventTrackRecordArm e( track->ID, 0.0f );
		writeToDspRingbuffer( &e );
	}
	//printf("track %i record Arm %s\n", track->ID, b ? "off" : "on" );
}

void gtrack_jacksendactivate_cb(Fl_Widget* w,void *data)
{
	GTrack* track = (GTrack*) data;
	Avtk::LightButton* d = (Avtk::LightButton*)w;
	bool b=d->value();
	// d->value(!b);
	if ( b < 0.5 ) {
		EventTrackJackSendActivate e( track->ID, 1.0f );
		writeToDspRingbuffer( &e );
	} else {
		EventTrackJackSendActivate e( track->ID, 0.0f );
		writeToDspRingbuffer( &e );
	}
}

