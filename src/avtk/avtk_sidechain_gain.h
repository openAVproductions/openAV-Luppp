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


#ifndef AVTK_SIDECHAIN_GAIN_H
#define AVTK_SIDECHAIN_GAIN_H

#include <FL/Fl_Widget.H>
#include <FL/Fl_Slider.H>
#include <valarray>
#include <string>

#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace Avtk
{

class SidechainGain : public Fl_Slider
{
public:
	SidechainGain(int _x, int _y, int _w, int _h, const char *_label = 0):
		Fl_Slider(_x, _y, _w, _h)
	{
		copy_label(_label);
		x = _x;
		y = _y;
		w = _w;
		h = _h;

		mouseClickedX = 0;
		mouseClickedY = 0;
		mouseClicked = false;

		active = true;
		highlight = false;

		_threshold  = 1.0;
		_target     = 1.f;
		_reduce     = 1.f;
		_release    = 0.5;

		_sidechainAmp = 0;
	}

	void threshold(float t)
	{
		_threshold = t;
		redraw();
	}
	void reduce   (float r)
	{
		_reduce    = r;
		redraw();
	}
	void release  (float r)
	{
		_release   = r;
		redraw();
	}

	/// sets the sidechain amplitude
	void sidechain(float s)
	{
		_sidechainAmp = s;
		redraw();
	}

	bool active;
	bool highlight;
	int x, y, w, h;

	int mouseClickedX;
	int mouseClickedY;
	bool mouseClicked;
	bool mouseRightClicked;

	float _threshold;
	float _target;
	float _reduce;
	float _release;

	float _sidechainAmp;

	void set_active(bool a)
	{
		active = a;
		redraw();
	}

	void draw()
	{
		if (damage() & FL_DAMAGE_ALL) {
			cairo_t *cr = Fl::cairo_cc();

			cairo_save( cr );

			cairo_set_line_width(cr, 1.5);


			// fill background
			cairo_rectangle( cr, x, y, w, h);
			cairo_set_source_rgb( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f  );
			cairo_fill_preserve( cr );
			cairo_clip( cr );


			// set up dashed lines, 1 px off, 1 px on
			double dashes[1];
			dashes[0] = 2.0;

			cairo_set_dash ( cr, dashes, 1, 0.0);
			cairo_set_line_width( cr, 1.0);

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


			// draw threshold / ducked line
			cairo_move_to( cr, x + w * 0.750 - (w * 0.5 * (1-_threshold)), y  );

			cairo_line_to( cr, x + w * 0.750 - (w * 0.5 * (1-_threshold)) + _sidechainAmp* _reduce*( w * 0.5 ), y + h / 2 );

			cairo_line_to( cr, x + w * 0.750 - (w * 0.5 * (1-_threshold)), y + h );

			cairo_line_to( cr, x + w , y + h );
			cairo_line_to( cr, x + w , y );
			cairo_close_path( cr );
			cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.21 );
			cairo_fill_preserve( cr );
			cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
			cairo_stroke( cr );

			// _sidechainAmp => arrow
			cairo_move_to( cr, x + w * 0.00 * _sidechainAmp, y + h * 0.4 - (h*0.1*_sidechainAmp) );
			cairo_line_to( cr, x + w * 0.65 * _sidechainAmp, y + h * 0.4 );
			cairo_line_to( cr, x + w * 0.1 + w * 0.65 * _sidechainAmp, y + h * 0.5 );
			cairo_line_to( cr, x + w * 0.65 * _sidechainAmp, y + h * 0.6 );
			cairo_line_to( cr, x + w * 0.00 * _sidechainAmp, y + h * 0.6 + (h*0.1*_sidechainAmp) );
			cairo_close_path( cr );
			cairo_set_source_rgba( cr, 1.0, 0.48, 0.f , 0.21 );
			cairo_fill_preserve( cr );
			cairo_set_source_rgba( cr, 1.0, 0.48, 0.f , 1 );
			cairo_stroke( cr );


			// _release horizontal line
			cairo_move_to( cr, x    , y + h * 0.25 + h/2 * _release );
			cairo_line_to( cr, x + w, y + h * 0.25 + h/2 * _release );
			cairo_set_source_rgba( cr, 1.0, 0.0, 0.f , 1 );
			cairo_stroke( cr );

			// stroke outline
			cairo_rectangle(cr, x, y, w, h);
			cairo_set_source_rgba( cr,  126 / 255.f,  126 / 255.f ,  126 / 255.f , 0.8 );
			cairo_set_line_width(cr, 1.9);
			cairo_stroke( cr );

			if ( !active ) {
				// big grey X
				cairo_set_line_width(cr, 20.0);
				cairo_set_source_rgba(cr, 0.4,0.4,0.4, 0.7);

				cairo_move_to( cr, x + (3 * w / 4.f), y + ( h / 4.f ) );
				cairo_line_to( cr, x + (w / 4.f), y + ( 3 *h / 4.f ) );

				cairo_move_to( cr, x + (w / 4.f), y + ( h / 4.f ) );
				cairo_line_to( cr, x + (3 * w / 4.f), y + ( 3 *h / 4.f ) );
				cairo_set_line_cap ( cr, CAIRO_LINE_CAP_BUTT);
				cairo_stroke( cr );
			}

			cairo_restore( cr );
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
		//TODO needed?

		return 0;
	}

private:
};

} // Avtk

#endif // AVTK_SIDECHAIN_GAIN_H
