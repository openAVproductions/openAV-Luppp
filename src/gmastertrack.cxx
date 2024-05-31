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

#include "gmastertrack.hxx"

#include <FL/Fl_Menu_Item.H>

static void gmastertrack_tempoDial_callback(Fl_Widget *w, void *data)
{
	Avtk::Dial* b = (Avtk::Dial*)w;
	float bpm = (int)(b->value() * (float)(MAX_TEMPO - MIN_TEMPO) + MIN_TEMPO);
	if(std::fabs(bpm-round(bpm))) {
		LUPPP_WARN("%f",bpm);
	}
	EventTimeBPM e = EventTimeBPM( bpm );
	writeToDspRingbuffer( &e );
}

static void gmastertrack_volume_callback(Fl_Widget *w, void *data)
{
	Avtk::Volume* b = (Avtk::Volume*)w;
	float v = b->value();
	EventMasterVol e( v );
	writeToDspRingbuffer( &e );
}

static void gmastertrack_inputVolume_callback(Fl_Widget *w, void *data)
{
	Avtk::Volume* b = (Avtk::Volume*)w;
	float v = b->value();
	EventMasterInputVol e( v );
	writeToDspRingbuffer( &e );
}

static void gmastertrack_sidchainKeyButton_callback(Fl_Widget *w, void *data)
{
	Avtk::LightButton* b = (Avtk::LightButton*)w;
	b->value( !b->value() );
	EventMasterInputToActive e( INPUT_TO_SIDE_KEY, b->value() );
	writeToDspRingbuffer( &e );
	//printf("Key button\n");
}

static void gmastertrack_mixButton_callback(Fl_Widget *w, void *data)
{
	Avtk::LightButton* b = (Avtk::LightButton*)w;
	b->value( !b->value() );
	EventMasterInputToActive e( INPUT_TO_MIX, b->value() );
	writeToDspRingbuffer( &e );
	//printf("Mix button\n");
}

static void gmastertrack_sendButton_callback(Fl_Widget *w, void *data)
{
	Avtk::LightButton* b = (Avtk::LightButton*)w;
	b->value( !b->value() );
	EventMasterInputToActive e( INPUT_TO_SEND, b->value() );
	writeToDspRingbuffer( &e );
	//printf("Send button\n");

}

static void gmastertrack_sendVol_callback(Fl_Widget *w, void *data)
{
	Avtk::Dial* b = (Avtk::Dial*)w;
	float v = b->value();
	EventMasterInputTo e = EventMasterInputTo( INPUT_TO_SEND, v );
	writeToDspRingbuffer( &e );
	///printf("Send dial\n");
}

static void gmastertrack_xSideVol_callback(Fl_Widget *w, void *data)
{
	Avtk::Dial* b = (Avtk::Dial*)w;
	float v = b->value();
	EventMasterInputTo e = EventMasterInputTo( INPUT_TO_XSIDE, v );
	writeToDspRingbuffer( &e );
	//printf("XSide dial\n");
}

/// return volume
static void gmastertrack_returnVol_callback(Fl_Widget *w, void *data)
{
	Avtk::Dial* b = (Avtk::Dial*)w;
	float v = b->value();
	EventMasterReturn e( RETURN_MAIN, v );
	writeToDspRingbuffer( &e );
	//printf("Return dial\n");
}

static void gmastertrack_mixVol_callback(Fl_Widget *w, void *data)
{
	Avtk::Dial* b = (Avtk::Dial*)w;
	float v = b->value();
	EventMasterInputTo e = EventMasterInputTo( INPUT_TO_MIX, v );
	writeToDspRingbuffer( &e );
}

static void gmastertrack_transport_callback(Fl_Widget *w, void *data)
{
	Avtk::LightButton* b = (Avtk::LightButton*)w;
	if( b->value() ) {
		EventTransportState e = EventTransportState( TRANSPORT_ROLLING );
		writeToDspRingbuffer( &e );
		w->label( "@#||" );  // Changed stop to pause symbol
		b->value( 0 );
	} else {
		EventTransportState e = EventTransportState( TRANSPORT_STOPPED );
		writeToDspRingbuffer( &e );
		w->label( "@#>" );  // play symbol
		b->value( 1 );
	}

}

static void gmastertrack_button_callback(Fl_Widget *w, void *data)
{
	if ( strcmp( w->label(), "Metro" ) == 0 ) {
		if ( Fl::event_button() == FL_RIGHT_MOUSE ) {
			// popup volume menu: 10 "steps of volume"
			Fl_Menu_Item rclick_menu[] = {
				{ "Vol 100%" },
				{ "Vol  75%" },
				{ "Vol  50%" },
				{ "Vol  25%"},
				{ 0 }
			};

			Fl_Menu_Item *m = (Fl_Menu_Item*) rclick_menu->popup( Fl::event_x(), Fl::event_y(), 0, 0, 0);

			float v = 0.f;
			if ( !m ) {
				return;
			} else if ( strcmp(m->label(), "Vol 100%") == 0 ) {
				v = 1;
			} else if ( strcmp(m->label(), "Vol  75%") == 0 ) {
				v = 0.75;
			} else if ( strcmp(m->label(), "Vol  50%") == 0 ) {
				v = 0.5;
			} else
				v = 0.25;

			LUPPP_NOTE("metro vol = %f", v );

			EventMetronomeVolume e( v );
			writeToDspRingbuffer( &e );
		} else {
			Avtk::LightButton* b = (Avtk::LightButton*)w;
			b->value( !b->value() );
			EventMetronomeActive e = EventMetronomeActive( b->value() );
			writeToDspRingbuffer( &e );
		}



	} else if ( strcmp( w->label(), "Tap" ) == 0 ) {
		if ( Fl::event_button() == FL_RIGHT_MOUSE ) {
			const char* answer = fl_input("Enter BPM value (range %d and %d):", 0, MIN_TEMPO, MAX_TEMPO);
			if(answer) {
				int bpm = atoi(answer);
				
				if ( bpm >= MIN_TEMPO && bpm <= MAX_TEMPO) {
					EventTimeBPM e = EventTimeBPM( bpm );
					writeToDspRingbuffer( &e );
				}
			}
		} else {
			EventTimeTempoTap e;
			writeToDspRingbuffer( &e );
		}
	} else {
		LUPPP_WARN("Error: unknown command string");
	}
}

#define OFST 33
GMasterTrack::GMasterTrack(int x, int y, int w, int h, const char* l ) :
	Fl_Group(x, y, w, h),
	title( strdup(l) ),
	bg( x, y , w, h, title ),

	// with "true" master flag: launches scenes instead of clips on tracks
	clipSel(x + 5, y + 26 + 102, 140, 294,"", true),

	source(x+5, y+26, 140, 100, ""),
	volBox(x+5, y+422, 140, 232, ""),

	transport      ( x + w * 2/4.f - 18, y + 436 + OFST * 0, 44,28, "Stop" ),
	tapTempo       ( x + w * 2/4.f - 18, y + 436 + OFST * 1, 44,28, "Tap" ),
	metronomeButton( x + w * 2/4.f - 18, y + 436 + OFST * 2, 44,28,"Metro"),

	tempoDial      ( x + w * 2/4.f - 18, y + 436 + OFST * 3.5, 45, 38,"BPM"),
	returnVol      ( x + w * 2/4.f - 18, y + 436 + OFST * 5, 45, 38,"Return"),

	inputVolume(x + 9,y + 26 + 4, w - 18, 30,""),

	inputToSend   (x + 10,y + 28 + 68, 40, 26,"Snd"),
	inputToSendVol(x + w*0.2-15,y + 28 + 36, 30, 30,""),

	inputToSidechainKey      (x + w*0.5-20,y + 28 + 68, 40, 26,"Key"),
	inputToSidechainSignalVol(x + w*0.5-15,y + 28 + 36, 30, 30,""),

	inputToMix   (x + w*0.8-20,y + 28 + 68, 40, 26,"Mix"),
	inputToMixVol(x + w*0.8-15,y + 28 + 36, 30, 30,""),

	volume(x+106, y +425, 36, 216, "")
{
	ID = privateID++;

	bar = 0;

	inputVolume.value(0.5);
	inputVolume.setOrientationHorizontal();

	inputVolume.callback( gmastertrack_inputVolume_callback, 0 );

	transport.callback( gmastertrack_transport_callback, &ID );

	tapTempo.callback( gmastertrack_button_callback, &ID );

	metronomeButton.callback( gmastertrack_button_callback, 0 );

	tempoDial.callback( gmastertrack_tempoDial_callback, 0 );

	inputToSend.setColor( 0, 1.0, 0 );
	inputToSend.callback( gmastertrack_sendButton_callback, 0 );
	inputToSendVol.callback( gmastertrack_sendVol_callback, 0 );

	inputToSidechainKey.setColor( 0, 0.6, 1 );
	inputToSidechainKey.callback( gmastertrack_sidchainKeyButton_callback, 0 );
	inputToSidechainSignalVol.value( 0 );
	inputToSidechainSignalVol.callback( gmastertrack_xSideVol_callback, 0 );

	inputToMix.callback   ( gmastertrack_mixButton_callback, 0 );
	inputToMixVol.callback   ( gmastertrack_mixVol_callback, 0 );

	tempoDial.align( FL_ALIGN_CENTER );

	returnVol.value( 1.f );
	returnVol.align( FL_ALIGN_CENTER );
	returnVol.callback( gmastertrack_returnVol_callback, 0 );

	for(int i = 0; i < 4; i++) {
		beatLights[i]   = new Avtk::LightButton( x + 10, y + 437 + 54 * i, 40, 42, "" );
	}
	beatLights[0]->setColor( 1.0, 0.0 , 0.0 );
	beatLights[1]->setColor( 1.0, 0.48, 0.0 );
	beatLights[2]->setColor( 1.0, 1.0 , 0.0 );
	beatLights[3]->setColor( 0.0, 1.0 , 0.0 );

	volume.amplitude( 0.0, 0.0 );
	volume.callback( gmastertrack_volume_callback, 0 );

	end(); // close the group
}

void GMasterTrack::setBpm( int b )
{
	bpm = b;
	tempoDial.value( ( bpm - MIN_TEMPO ) / (float)(MAX_TEMPO - MIN_TEMPO) );
	std::stringstream s;
	s << bpm;
	tempoDial.copy_label( s.str().c_str() );
}

void GMasterTrack::setInputVol(float f)
{
	//LUPPP_NOTE(" gmtrck, inputVol %f", f );
	inputVolume.value( f );
}

void GMasterTrack::setReturnVol(float f)
{
	LUPPP_NOTE(" gmtrck, returnVol %f", f );
	returnVol.value( f );
}

void GMasterTrack::setInputTo(int to, float f)
{
	//LUPPP_NOTE(" gmtrck, inputTO %i, %f", to, f );
	if ( to == Event::INPUT_TO_MIX )
		inputToMixVol.value( f );
	else if ( to == Event::INPUT_TO_SEND )
		inputToSendVol.value( f );
	else if ( to == Event::INPUT_TO_XSIDE )
		inputToSidechainSignalVol.value( f );
}

void GMasterTrack::setInputToActive(int to, bool f)
{
	//LUPPP_NOTE(" gmtrck, inputToActive %i, %i", to, int(f) );

	if ( to == Event::INPUT_TO_MIX )
		inputToMix.value( f );
	else if ( to == Event::INPUT_TO_SEND )
		inputToSend.value( f );
	else if ( to == Event::INPUT_TO_SIDE_KEY )
		inputToSidechainKey.value( f );
}

void GMasterTrack::metronomeEnable( bool b )
{
	metronomeButton.value( b );
}

int GMasterTrack::getBpm()
{
	return bpm;
}

void GMasterTrack::setTapTempo( bool b )
{
	tapTempo.setHighlight( b );
}

void GMasterTrack::setBarBeat(int b, int beat)
{
	// FIXME: hard coded 4/4 time here
	if ( beat % 4 == 0 ) {
		bar = bar % 4 + 1;
	}

	// turn all off
	for( int i = 0; i < 4; i++)
		beatLights[i]->value( 0 );

	// beat starts at 4
	// FIXME: hard coded 4/4 time
	beatLights[ 3 - beat%4 ]->value( 1 );
}

Avtk::Volume* GMasterTrack::getInputVolume()
{
	return &inputVolume;
}
Avtk::Volume* GMasterTrack::getVolume()
{
	return &volume;
}

Avtk::LightButton* GMasterTrack::getInputToSend()
{
	return &inputToSend;
}
Avtk::LightButton* GMasterTrack::getInputToSidechainKey()
{
	return &inputToSidechainKey;
}
Avtk::LightButton* GMasterTrack::getInputToMix()
{
	return &inputToMix;
}

Avtk::Dial* GMasterTrack::getInputToSendVol()
{
	return &inputToSendVol;
}
Avtk::Dial* GMasterTrack::getInputToXSide()
{
	return &inputToSidechainSignalVol;
}
Avtk::Dial* GMasterTrack::getInputToMixVol()
{
	return &inputToMixVol;
}


Avtk::ClipSelector* GMasterTrack::getClipSelector()
{
	return &clipSel;
}

GMasterTrack::~GMasterTrack()
{
	free(title);
}
