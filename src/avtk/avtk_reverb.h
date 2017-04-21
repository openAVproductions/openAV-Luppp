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


#ifndef AVTK_REVERB_H
#define AVTK_REVERB_H

#include <FL/Fl_Slider.H>

namespace Avtk
{

class Reverb : public Fl_Slider
{
public:
	Reverb(int _x, int _y, int _w, int _h, const char *_label =0):
		Fl_Slider(_x, _y, _w, _h)
	{
		copy_label(_label);
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		amp = 0.5;
		s   = 0.5;
		damp= 0.5;

		active = false;

		highlight = false;
		mouseOver = false;
	}

	void size(float v)
	{
		s = v;
		redraw();
	}
	void wet(float v)
	{
		amp = v;
		redraw();
	}
	void damping(float v)
	{
		damp = v;
		redraw();
	}

	float size()
	{
		return s;
	}
	float wet ()
	{
		return amp;
	}
	float damping()
	{
		return damp;
	}

	bool getActive()
	{
		return active;
	}
	void setActive(bool a)
	{
		active = a;
		redraw();
	}

	float s;
	float amp;
	float damp;

	bool active;

	bool mouseOver;
	bool highlight;
	int x, y, w, h;

	void draw()
	{
		if (damage() & FL_DAMAGE_ALL) {
			cairo_t *cr = Fl::cairo_cc();

			cairo_save( cr );

			// graph
			cairo_rectangle( cr, x, y, w, h );
			cairo_set_source_rgb( cr,28 / 255.f,  28 / 255.f ,  28 / 255.f  );
			cairo_fill(cr);


			// set up dashed lines, 1 px off, 1 px on
			double dashes[1];
			dashes[0] = 2.0;

			cairo_set_dash ( cr, dashes, 1, 0.0);

			// loop over each 2nd line, drawing dots
			cairo_set_line_width(cr, 1.0);
			cairo_set_source_rgb(cr, 0.4,0.4,0.4);
			for ( int i = 0; i < 4; i++ ) {
				cairo_move_to( cr, x + ((w / 4.f)*i), y );
				cairo_line_to( cr, x + ((w / 4.f)*i), y + h );
			}
			for ( int i = 0; i < 4; i++ ) {
				cairo_move_to( cr, x    , y + ((h / 4.f)*i) );
				cairo_line_to( cr, x + w, y + ((h / 4.f)*i) );
			}
			cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
			cairo_stroke(cr);
			cairo_set_dash ( cr, dashes, 0, 0.0);

			// draw "damping" control
			cairo_move_to( cr, x+w*0.1              , y + h*0.85 - (h*0.7*amp));
			cairo_line_to( cr, x+w*0.1 + (w-20)*damp, y + h*0.85 - (h*0.7*amp));
			cairo_set_source_rgba(cr, 1.0, 0.48,   0,  1);
			cairo_set_line_join( cr, CAIRO_LINE_JOIN_ROUND);
			cairo_set_line_cap ( cr, CAIRO_LINE_CAP_ROUND);
			cairo_set_line_width(cr, 1.9);
			cairo_stroke( cr );

			// draw reverb triangle
			cairo_move_to( cr, x , y + h*0.99 );
			cairo_line_to( cr, x + w*0.1, y + h*0.85 - (h*0.7*amp));
			cairo_line_to( cr, x + w*0.3+w*0.7*s, y + (h*0.99));

			// stroke
			cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.21 );
			cairo_fill_preserve(cr);
			cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
			cairo_set_line_width(cr, 1.2);
			cairo_stroke( cr );

			// stroke rim
			cairo_rectangle(cr, x, y, w, h);
			//cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
			cairo_set_source_rgba( cr,  126 / 255.f,  126 / 255.f ,  126 / 255.f , 0.8 );
			cairo_set_line_width(cr, 0.9);
			cairo_stroke( cr );

			if ( !active ) {
				// big grey X
				cairo_set_line_width(cr, 12.0);
				cairo_set_source_rgba(cr, 0.0,0.0,0.0, 1.0);

				cairo_move_to( cr, x + (3 * w / 4.f), y + ( h / 4.f ) );
				cairo_line_to( cr, x + (w / 4.f), y + ( 3 *h / 4.f ) );

				cairo_move_to( cr, x + (w / 4.f), y + ( h / 4.f ) );
				cairo_line_to( cr, x + (3 * w / 4.f), y + ( 3 *h / 4.f ) );
				cairo_set_line_cap ( cr, CAIRO_LINE_CAP_BUTT);
				cairo_stroke( cr );
			}

			cairo_restore( cr );

			draw_label();
		}
	}

	void resize(int X, int Y, int W, int H)
	{
		Fl_Widget::resize(X,Y,W,H);
		x = X;
		y = Y;
		w = W;
		h = H;
		redraw();
	}

	int handle(int event)
	{
		switch(event) {
		case FL_PUSH:
			highlight = 1;
			if ( Fl::event_button() == FL_RIGHT_MOUSE ) {
				active = !active;
				redraw();
				do_callback();
			}
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
			if (highlight) {
				highlight = 0;
				redraw();
			}
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
};

} // Avtk

#endif // AVTK_ADSR_H

