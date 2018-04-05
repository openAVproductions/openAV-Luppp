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


#include "clipselector.hxx"
#include <unistd.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include "../gui.hxx"
#define RECORD_BARS_MENU_ITEM(num) { #num, 0, setRecordBarsCb, (void*)num, FL_MENU_RADIO | ((clips[clipNum].getBeatsToRecord() == num*4) ? FL_ACTIVATE : 0 | empty ? 0 : FL_MENU_INACTIVE) }
#define RECORD_LENGTH_MENU_ITEM(num) {#num, 0, setLengthCb, (void*)num, empty ? FL_MENU_INACTIVE : 0}

extern Gui* gui;

namespace Avtk
{

ClipSelector::ClipSelector( int _x, int _y, int _w, int _h,
							const char *_label, bool master ) :
	Fl_Button(_x, _y, _w, _h)
{
	copy_label(_label);
	x = _x;
	y = _y;
	w = _w;
	h = _h;

	special = -1;

	_master = master;

	if ( _master ) {
		for(int i = 0; i < NSCENES; i++ ) {
			stringstream s;
			s << "Scene " << i + 1;
			clips[i].setName( s.str() );
		}

	}

	highlight = false;
	mouseOver = false;
}



void ClipSelector::setID( int id )
{
	ID = id;
}



void ClipSelector::setState( int clipNum, GridLogic::State s )
{
#ifdef DEBUG_CLIP
	//cout << "ClipSelector::setState() t = " << ID << " clipNum = " << clipNum << "  state = " << s << endl;
#endif
	clips[clipNum].setState( s );
	if ( s == GridLogic::STATE_EMPTY ) {
		clips[clipNum].setName("");
	}

	redraw();
}

std::string ClipSelector::clipName(int clip)
{
	return clips[clip].getName();
}

void ClipSelector::clipName(int clip, std::string name)
{
	clips[clip].setName( name );
	redraw();
}

void ClipSelector::setClipBeats(int scene, int beats, bool isBeatsToRecord)
{
    clips[scene].setBeats(beats, isBeatsToRecord);
    redraw();
}

double getCairoTextWith(cairo_t * cr, const char * str)
{
    cairo_text_extents_t ex;
    cairo_text_extents(cr, str, &ex);
    return ex.width;
}

void ClipSelector::setSpecial(int scene)
{
	if ( special == -1 && scene == -1 ) {
		// no change
		return;
	}

	special = scene;
	redraw();
}

void ClipSelector::draw()
{
	if (damage() & FL_DAMAGE_ALL) {
		if ( value() ) {
			highlight = true;
		} else {
			highlight = false;
		}

		cairo_t *cr = Fl::cairo_cc();

		cairo_save( cr );

		int clipWidth  = w - 2;
		int clipHeight = (h / numClips);

		// text height adjustment based on clip size
		// small 22, 13
		// start 29, 17
		//printf("clipHeight %i\n", clipHeight);
		int textHeight = 13 + ((clipHeight - 22) * 0.66 );



		int xOff = x+clipHeight/2;

		int drawY = y + 1;
		for( int i = 0; i < numClips; i++) { // draw each clip
			int yOff = drawY+ clipHeight/2 -1;

			cairo_rectangle( cr, x+2, drawY, clipWidth-1, clipHeight - 4 );
			cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 0.4);
			cairo_fill(cr);

			cairo_rectangle( cr, x+2, drawY, clipHeight-4, clipHeight - 4 );

			if ( clips[i].getState() == GridLogic::STATE_RECORDING ) {
				cairo_set_source_rgba(cr, 1.f,  0 / 255.f ,  0 / 255.f, 1.f);
				cairo_fill(cr);
				//cairo_arc( cr, x+14, drawY+13, 4.3, 0, 6.29 );
				cairo_arc( cr, xOff, yOff, 4.3, 0, 6.29 );
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_fill(cr);
			} else if ( clips[i].getState() == GridLogic::STATE_PLAYING ) {
				cairo_set_source_rgba(cr, 0.0, 1.0,   0, 1.f );
				cairo_fill(cr);
				cairo_move_to( cr, xOff-4, yOff-5 );
				cairo_line_to( cr, xOff+5, yOff   );
				cairo_line_to( cr, xOff-4, yOff+5 );
				cairo_close_path(cr);
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_fill(cr);
			} else if ( clips[i].getState() == GridLogic::STATE_PLAY_QUEUED ) {
				cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
				cairo_fill(cr);
				cairo_move_to( cr, xOff-4, yOff-5 );
				cairo_line_to( cr, xOff+5, yOff   );
				cairo_line_to( cr, xOff-4, yOff +5 );
				cairo_close_path(cr);
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_fill(cr);
			} else if ( clips[i].getState() == GridLogic::STATE_STOP_QUEUED ) {
				cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
				cairo_fill(cr);
				cairo_arc( cr, xOff, yOff, 4.3, 0, 6.29 );
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_set_line_width(cr, 2.2f);
				cairo_stroke(cr);
			} else if ( clips[i].getState() == GridLogic::STATE_RECORD_QUEUED ) {
				cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
				cairo_fill(cr);
				cairo_arc( cr, xOff, yOff, 4.3, 0, 6.29 );
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_fill(cr);
			} else if ( clips[i].getState() == GridLogic::STATE_STOPPED ) {
				cairo_set_source_rgba(cr, 1.0, 0.6,   0, 1.f);
				cairo_fill(cr);
				cairo_arc( cr, xOff, yOff, 4.3, 0, 6.29 );
				cairo_set_source_rgba(cr, 0, 0, 0, 1.f);
				cairo_set_line_width(cr, 2.2f);
				cairo_stroke(cr);
			} else {
				cairo_set_source_rgba(cr, 66 / 255.f,  66 / 255.f ,  66 / 255.f, 1.f);
				cairo_fill(cr);
			}

			cairo_rectangle( cr, x+2, drawY, clipWidth -1, clipHeight - 3 );

			float alpha = 1;
			cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, alpha);
			cairo_set_line_width( cr, 1.3);
			cairo_move_to( cr, x+clipHeight-1, drawY );
			cairo_line_to( cr, x+clipHeight-1, drawY + clipHeight - 2);
			cairo_stroke(cr);

            int beatLen = 0;

            // clip bars
            if(!_master) {
                const char * bars = clips[i].getBarsText();
                const char * beats = clips[i].getBeatsText();
                bool toRecord = clips[i].getBeats() <= 0 && clips[i].getBeatsToRecord() > 0; // If there are BeatsToRecord, but no Beats

                if(strlen(bars)) {
                    if(toRecord) cairo_set_source_rgba(cr, 1.f,  0 / 255.f ,  0 / 255.f, 1.f);
                    else cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f, 255 / 255.f , 0.9 );

                    cairo_move_to( cr,  x + clipWidth - 5 - getCairoTextWith(cr, bars), drawY + textHeight - 8);
                    cairo_set_font_size( cr, 11 );
                    cairo_show_text( cr, bars);

                    beatLen =  getCairoTextWith(cr, beats);
                    cairo_move_to( cr, x + clipWidth -  5 - beatLen, drawY + textHeight + 7);
                    cairo_show_text( cr, beats);
                }
            }

			// clip name
			cairo_move_to( cr, x + clipHeight + 5, drawY + textHeight );
			cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 0.9 );
			cairo_set_font_size( cr, 11 );

            std::string tmp = clips[i].getName();

            // trim the names
            while(getCairoTextWith(cr, tmp.c_str()) > clipWidth - (clipHeight + 15 + beatLen)){
                if(tmp.length() < 4){
                    tmp = "";
                    break;
                }
                tmp = tmp.substr(0, tmp.length() - 4);
                tmp += "…";
            }

            cairo_show_text( cr, tmp.c_str() );

			// special indicator
			if ( i == special ) {
				cairo_rectangle( cr, x+2, drawY, clipWidth -1, clipHeight - 3 );
				cairo_set_source_rgba(cr, 0.0, 153 / 255.f, 1.0, alpha);
				cairo_stroke( cr );
			}

			drawY += clipHeight;

		}

		cairo_restore( cr );

		//draw_label();
	}
}

void ClipSelector::resize(int X, int Y, int W, int H)
{
	Fl_Widget::resize(X,Y,W,H);
	x = X;
	y = Y;
	w = W;
	h = H;
	redraw();
}

void setRecordBarsCb(Fl_Widget *w, void* data)
{
    if(!w || !data)
        return;

	ClipSelector *track = (ClipSelector*)w;
	long bars = (long)data;
	if(bars == -2){        
		const char* answer = fl_input("Enter a custom number: ");
        if(!answer || atoi(bars) <= 0|| atoi(bars) > MAX_BARS) {
            bars = -1;
            fl_message("Please enter value between 1 and %.", MAX_BARS);
        } else
			bars = atoi(answer);
	}

	EventLooperBarsToRecord e(track->ID, track->getLastClipNum(), bars);
	writeToDspRingbuffer( &e );
}

void setLengthCb(Fl_Widget *w, void* data)
{
	ClipSelector *track = (ClipSelector*)w;
	long beats = (long)data;
	EventLooperLoopLength e(track->ID, track->getLastClipNum(), beats);
	writeToDspRingbuffer( &e );
}

int ClipSelector::findClipNum() {
	// calculate the clicked clip number
	int clipHeight = (h / numClips);
	clipNum = ( (Fl::event_y() ) - y ) / clipHeight;
	if (clipNum >= numClips)
		clipNum = numClips -1; // fix for clicking the lowest pixel

	return clipNum;
}

int ClipSelector::getLastClipNum()
{
	return clipNum;
}

int ClipSelector::handle(int event)
{
	switch(event) {
	case FL_ACTIVATE: {
	}
	case FL_DEACTIVATE: {
	}
	return 1;
	case FL_PUSH:
		highlight = 1;
		{
			findClipNum();

			// handle right clicks: popup menu
			if ( Fl::event_state(FL_BUTTON3) ) {
				if ( _master ) {
					// ask new name for clip, then
					const char* name = fl_input( "Scene name: ", clips[clipNum].getName().c_str() );
					if ( name )
						clips[clipNum].setName( name );

					redraw();
					return 1;
				}


                bool empty =  clips[clipNum].getState() == GridLogic::STATE_EMPTY;
				Fl_Menu_Item rclick_menu[] = {
					{ "Load" },
					{ "Save" },
					{ "Special"},
                    { "Beats",  0,   0, 0, FL_SUBMENU | FL_MENU_DIVIDER },
					RECORD_LENGTH_MENU_ITEM(1),
					RECORD_LENGTH_MENU_ITEM(2),
					RECORD_LENGTH_MENU_ITEM(4),
					RECORD_LENGTH_MENU_ITEM(8),
					RECORD_LENGTH_MENU_ITEM(16),
					RECORD_LENGTH_MENU_ITEM(32),
					RECORD_LENGTH_MENU_ITEM(64),
					{0},
                    { "Bars to record",  0,   0, 0, FL_SUBMENU | FL_MENU_DIVIDER},
					RECORD_BARS_MENU_ITEM(1),
					RECORD_BARS_MENU_ITEM(2),
					RECORD_BARS_MENU_ITEM(4),
					RECORD_BARS_MENU_ITEM(6),
					RECORD_BARS_MENU_ITEM(8),
                    {"Endless", 0, setRecordBarsCb, (void*)-1, FL_MENU_DIVIDER | FL_MENU_RADIO | ((clips[clipNum].getBeatsToRecord() <= 0) ? FL_ACTIVATE : 0)
                     | empty ? 0 : FL_MENU_INACTIVE},
                    {"Custom", 0, setRecordBarsCb, (void*)-2, empty ? 0 : FL_MENU_INACTIVE},
					{0},
					//{ "Record" },
					{ "Use as tempo" },
					{ "Rename", 0, 0, 0, FL_MENU_DIVIDER},
					{ "Clear" },
					{ 0 }
				};
				Fl_Menu_Item *m = (Fl_Menu_Item*) rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
				if ( !m ) {
					return 0;
				} else if ( strcmp(m->label(), "Load") == 0 ) {
					gui->selectLoadSample( ID, clipNum );
				} else if ( strcmp(m->label(), "Save") == 0 ) {
					//gui->saveBufferPath = "/tmp/test.wav";
					char* tmp = gui->selectSavePath();
					if(tmp && strlen(tmp)) {
						if( access( tmp, F_OK ) != -1 ) {
							int overwrite = fl_choice("Overwrite file?","Cancel","Overwrite",0);
							if (!overwrite) {
								return 0;
							}
						}
						gui->saveBufferPath = tmp;
						free(tmp);
						gui->selectSaveSample( ID, clipNum );
					}
				} else if ( strcmp(m->label(), "Use as tempo") == 0 ) {
					EventLooperUseAsTempo e (ID, clipNum);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "Special") == 0 ) {
					//printf("special selected %i, %i\n", ID, clipNum );
					EventGridSelectNewChosen e( ID, clipNum);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "Rename") == 0 ) {
					const char* name = fl_input( "Clip name: ", clips[clipNum].getName().c_str() );
					if ( name )
						clips[clipNum].setName( name );
				} else if ( strcmp(m->label(), "Clear") == 0 ) {
					// for a clip to become 0
					EventGridState e( ID, clipNum, GridLogic::STATE_EMPTY );
					writeToDspRingbuffer( &e );
				} else {
					 m->do_callback(this, m->user_data());
				}
			} else {
				if ( _master ) {
					EventGridLaunchScene e( clipNum );
					writeToDspRingbuffer( &e );
					redraw();
					return 1;
				}

				// write "pressed" event for this track,scene
				EventGridEvent e( ID, clipNum, true );
				writeToDspRingbuffer( &e );
			}
		}
		redraw();
		do_callback();
		return 1;
	case FL_DRAG: {
		int t = Fl::event_inside(this);
		if (t != highlight) {
			highlight = t;
			redraw();
		}
	}
	return 1;
	case FL_ENTER:
		mouseOver = true;

		redraw();
		return 1;
	case FL_LEAVE:
		mouseOver = false;
		redraw();
		return 1;
	case FL_RELEASE:
		return 1;
	case FL_SHORTCUT:
		if ( test_shortcut() ) {
			do_callback();
			return 1;
		}
		return 0;
	default:
		return Fl_Widget::handle(event);
	}
}

void ClipState::setBeats(int numBeats, bool isBeatsToRecord)
{
	if(numBeats > 0) {
        beatsText = std::to_string(numBeats);
        barsText = std::to_string(numBeats/4);
        if(isBeatsToRecord){
              beatsToRecord = numBeats;
              beats = 0;
        } else {
            beats = numBeats;
            if(numBeats <= 0)
                beatsToRecord = -1;
        }
	}
	else
        barsText = beatsText = std::string("");
}
} // Avtk
