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

			// clip name
			cairo_move_to( cr, x + clipHeight + 5, drawY + textHeight );
			cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 0.9 );
			cairo_set_font_size( cr, 11 );

			std::string tmp = clips[i].getName().substr(0,8);

			cairo_show_text( cr, tmp.c_str() );

			// special indicator?
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
			// calculate the clicked clip number
			int clipHeight = (h / numClips);
			int clipNum = ( (Fl::event_y() ) - y ) / clipHeight;
			if (clipNum >= numClips)
				clipNum = numClips -1; // fix for clicking the lowest pixel

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


				Fl_Menu_Item rclick_menu[] = {
					{ "Load" },
					{ "Save" },
					{ "Special"},
					{ "Beats",  0,   0, 0, FL_SUBMENU | FL_MENU_DIVIDER },
					{"1       "},
					{"2"},
					{"4"},
					{"8"},
					{"16"},
					{"32"},
					{"64"},
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
				} else if ( strcmp(m->label(), "1       ") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,1);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "2") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,2);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "4") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,4);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "8") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,8);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "16") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,16);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "32") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,32);
					writeToDspRingbuffer( &e );
				} else if ( strcmp(m->label(), "64") == 0 ) {
					EventLooperLoopLength e = EventLooperLoopLength(ID, clipNum ,64);
					writeToDspRingbuffer( &e );
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

} // Avtk

