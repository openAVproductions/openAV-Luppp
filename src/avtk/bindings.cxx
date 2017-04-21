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


#include "bindings.h"

#include <sstream>

#include "../event.hxx"

namespace Avtk
{

Bindings::Bindings( int _x, int _y, int _w, int _h, const char *_label ) :
	Fl_Button(_x, _y, _w, _h)
{
	copy_label(_label);
	x = _x;
	y = _y;
	w = _w;
	h = _h;

	bindYPx = 25;

	highlight = false;
	mouseOver = false;
}


void Bindings::draw()
{
	if (damage() & FL_DAMAGE_ALL) {
		cairo_t *cr = Fl::cairo_cc();
		cairo_save( cr );

		cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 1 );
		cairo_set_font_size( cr, 11 );
		cairo_move_to( cr, x + 7, y+15 );
		cairo_show_text( cr, "Action:" );

		cairo_move_to( cr, x + 140, y+15 );
		cairo_show_text( cr, "T:" );
		cairo_move_to( cr, x + 160, y+15 );
		cairo_show_text( cr, "S:" );


		cairo_move_to( cr, x + 200, y+15 );
		cairo_show_text( cr, "B1:" );
		cairo_move_to( cr, x + 230, y+15 );
		cairo_show_text( cr, "B2:" );


		int drawY = y + bindYPx;
		for(unsigned int i = 0; i < binds.size(); i++) { // draw each binding
			cairo_rectangle( cr, x+2, drawY, 179, 23 );
			cairo_set_source_rgba(cr, 0 / 255.f,  0 / 255.f ,  0 / 255.f, 0.4);
			cairo_fill(cr);

			// action
			cairo_move_to( cr, x + 7, drawY+15 );
			cairo_set_source_rgba( cr, 255 / 255.f, 255 / 255.f , 255 / 255.f , 1 );
			cairo_set_font_size( cr, 11 );
			const char* action = Event::getPrettyName( binds.at(i)->action );
			if ( action )
				cairo_show_text( cr, action );

			// track
			cairo_move_to( cr, x + 140, drawY+15 );
			{
				std::stringstream s;
				s << binds.at(i)->track;
				cairo_show_text( cr, s.str().c_str() );
			}
			// scene
			cairo_move_to( cr, x + 150, drawY+15 );
			{
				std::stringstream s;
				s << binds.at(i)->scene;
				cairo_show_text( cr, s.str().c_str() );
			}

			// status
			cairo_move_to( cr, x + 165, drawY+15 );
			{
				std::stringstream s;
				s << binds.at(i)->status;
				cairo_show_text( cr, s.str().c_str() );
			}
			// data
			cairo_move_to( cr, x + 175, drawY+15 );
			{
				std::stringstream s;
				s << binds.at(i)->data;
				cairo_show_text( cr, s.str().c_str() );
			}
			// move to next line
			drawY += bindYPx;
		}

		//printf("done drawing\n");

		cairo_restore( cr );
	}
}

void Bindings::add( Binding* b )
{
	// FIXME: Refactor to use copy-constructor?
	Binding* tmp = new Binding();
	tmp->action = b->action;
	tmp->status = b->status;
	tmp->data   = b->data;

	tmp->active = b->active;
	tmp->track  = b->track;
	tmp->scene  = b->scene;
	tmp->send   = b->send;

	binds.push_back( tmp );

	/*
	int height = binds.size() * bindYPx;
	printf("resize to %i, %i, %i, %i\n", x, y, w, height );
	resize( x, y, w, height );
	*/

	redraw();
}

void Bindings::resize(int X, int Y, int W, int H)
{
	Fl_Widget::resize(X,Y,W,H);
	x = X;
	y = Y;
	w = W;
	h = H;
	redraw();
}

} // namespace Avtk
