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


#include "gaudioeditor.hxx"

#include <sstream>

#include "config.hxx"
#include "gui.hxx"
extern Gui* gui;

#include "audiobuffer.hxx"

#include "avtk/waveform.h"
#include "avtk/avtk_button.h"


void oneCB      (Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 1 );
}
void twoCB      (Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 2 );
}
void fourCB     (Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 4 );
}
void eightCB    (Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 8 );
}
void sixteenCB  (Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 16);
}
void thirtyTwoCB(Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 32);
}
void sixtyfourCB(Fl_Widget*,void* data)
{
	((AudioEditor*)data)->setBeatsAndQuit( 64);
}

void cancelCB(Fl_Widget*,void* data)
{
	printf("button, beats = 4\n");
	AudioEditor* ae = (AudioEditor*) data;
	ae->setBeatsAndQuit( -1 );
}

AudioEditor::AudioEditor()
{
	window = new Fl_Double_Window(460,200,"Audio Editor : Beats?");
	waveform = new Avtk::Waveform(5, 5, 450, 150, "Waveform");
	cancel = new Avtk::Button(360, 160, 80,30, "Cancel");

	const char* names[] = {
		"1","2","4","8","16","32","64"
	};

	for(int i = 0; i < 7; i++) {
		stringstream s;
		s << i;
		beatButtons[i] = new Avtk::Button(5 + i * 50, 160, 40,30, names[i] );
	}
	window->end();

	beatButtons[0]->callback( oneCB, this);
	beatButtons[1]->callback( twoCB, this);
	beatButtons[2]->callback( fourCB, this);
	beatButtons[3]->callback( eightCB, this);
	beatButtons[4]->callback( sixteenCB, this);
	beatButtons[5]->callback( thirtyTwoCB, this);
	beatButtons[6]->callback( sixtyfourCB, this);

	cancel->callback( cancelCB, this );
}

void AudioEditor::show( AudioBuffer* buf, bool modal )
{
	ab = buf;

	if ( !ab ) {
		LUPPP_WARN("called with ab == 0");
		return;
	}

	std::vector<float>& tmp = ab->getDataL();
	const auto size = tmp.size();
	waveform->setData( tmp );

	const int beats[]= {1,2,4,8,16,32,64};

	int iBeatOne = -1;
	int iBeatTwo = -1;

	// figure out BPM values from size
	for( int i = 0; i < 7; i++ ) {
		int beat = beats[i];
		int fpb = size / beat;
		int bpm = (gui->samplerate / fpb) * 60;

		if ( bpm < 60 || bpm > 220 ) {
			// disable option: not valid
			beatButtons[i]->setGreyOut( true );
			beatButtons[i]->setColor( 0.4, 0.4, 0.4 );
		} else {
			//printf("%i, fpb = %i, bpm= = %i\n", beat, fpb, bpm );
			// enable option ( may be disabled previously! )
			beatButtons[i]->setGreyOut( false );

			// remember this beat was a valid one, to check for best match
			if ( iBeatOne == -1 )
				iBeatOne = i;
			else
				iBeatTwo = i;
		}
	}

	// both valid: compare, and adjust color
	if ( iBeatOne != -1 && iBeatTwo != -1 ) {
		int masterFpb = (gui->samplerate * 60) / gui->getMasterTrack()->getBpm();
		int oneFpb = size / beats[iBeatOne];
		int twoFpb = size / beats[iBeatTwo];

		int oneDelta = masterFpb - oneFpb;
		int twoDelta = masterFpb - twoFpb;

		if ( oneDelta < 0 ) oneDelta = -oneDelta;
		if ( twoDelta < 0 ) twoDelta = -twoDelta;

		if ( oneDelta == twoDelta ) {
			beatButtons[iBeatOne]->setColor( 0.0, 1.0, 0.0 );
			beatButtons[iBeatTwo]->setColor( 0.0, 1.0, 0.0 );
		} else if ( oneDelta <= twoDelta ) {
			// one is the better match
			beatButtons[iBeatOne]->setColor( 0.0, 1.0, 0.0 );
			beatButtons[iBeatTwo]->setColor( 1.0, 0.0, 0.0 );
		} else {
			beatButtons[iBeatTwo]->setColor( 0.0, 1.0, 0.0 );
			beatButtons[iBeatOne]->setColor( 1.0, 0.0, 0.0 );
		}
	} else if( iBeatOne != -1 && iBeatTwo == -1) { // only one valid
		beatButtons[iBeatOne]->setColor( 0.0, 1.0, 0.0 );
	} else {
		// no valid BPM range..?
	}

	window->set_modal();
	window->show();
}

void AudioEditor::hide()
{
	window->hide();
}


void AudioEditor::setBeatsAndQuit(int beats)
{
	ab->setBeats(beats);
	window->hide();
}

/*
AudioBuffer* AudioEditor::getAudioBuffer()
{
  return ab;
}
*/

bool AudioEditor::shown()
{
	return window->shown();
}

AudioEditor::~AudioEditor()
{
	delete window;
}
