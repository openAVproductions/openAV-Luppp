
/*
  This file is part of Luppp.
  Copyright 2012, Harry van Haaren

  Luppp is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Luppp is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Luppp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "g_instrumentselector.hpp"

#include "g_widgets.hpp"

#include <glibmm/fileutils.h>
//#include <gtkmm/filechooserdialog.h>

using namespace std;
using namespace Luppp;

InstrumentSelector::InstrumentSelector(Top* t, GuiStateStore* s)
{
  top = t;
  stateStore = s;
  
  //std::cout << "Enterin InstrumentSelector contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &InstrumentSelector::on_button_press_event) );
  
  // drag target types
  std::list<Gtk::TargetEntry> listTargets;
  listTargets.push_back( Gtk::TargetEntry("INSTRUMENT_STRING") );
  
  // drag source
  drag_source_set(listTargets);
  
  // drag function connect
  signal_drag_data_get().connect(sigc::mem_fun(*this, &InstrumentSelector::dragFunction ));
  
  // add these effect strings to the "selectable" list
  fileList.push_back( "Buffer player" );
  fileList.push_back( "White Noise gen" );
  fileList.push_back( "Fluidsynth player" );
  
  set_tooltip_text ( "Instrument Selector: Drag instruments from here to tracks" );
  
  set_size_request(200,18 * fileList.size() );
}

bool InstrumentSelector::redraw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
      Gdk::Rectangle r(0, 0, get_allocation().get_width(),
              get_allocation().get_height());
      win->invalidate_rect(r, false);
  }
  return true;
}


bool InstrumentSelector::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  if(window)    // Only run if Window does exist
  {
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width();
    int height = allocation.get_height();
    
    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();
    
    cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
    setColour(cr, COLOUR_GREY_3);
    cr->fill();
    
    // instruments available are hard coded in constructor
    std::list<std::string>::iterator i = fileList.begin();
    
    int x = 0;
    int y = 0;
    for(; i != fileList.end(); i++ )
    {
      // background colour
      setColour(cr, COLOUR_GREY_4);
      cr->set_line_width(0.9);
      cr->rectangle (x,y, event->area.width, 17);
      cr->fill();
      
      // draw "play square" on left of block
      setColour(cr, COLOUR_GREY_3);
      cr->rectangle (x+1, y+1, 15, 15);
      cr->fill();
      
      cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size ( 13 );
      cr->move_to ( x + 22, (y + 13) );
      setColour(cr, COLOUR_GREY_1);
      cr->show_text ( *i );
      
      
      y += 18;
    }
    
    
    /*
    if ( state.selected )
    {
      cr->rectangle(0, 0, 74, 102);
      setColour( cr, COLOUR_PURPLE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
    */
    
  }
  return true;
}

void InstrumentSelector::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void InstrumentSelector::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool InstrumentSelector::on_button_press_event(GdkEventButton* event)
{
  
  block = ((int)event->y) / 18;
  
  cout << "Button click on block " << block << endl;
  
  /*
  if( event->type == GDK_BUTTON_PRESS && event->button == 1 ) // left
  {
    // so we're gonna play a clip, if the track is record enabled, and the
    // current block is *NOT* recording yet, we set it to record, otherwise
    // we stop it from recording. If the track isn't record armed: then we
    // send a play message
    TrackOutputState* state = &stateStore->trackoutputState.at(ID);
    if ( state->recEnable && stateStore->trackoutputState.at(ID).recEnable == false )
    {
      std::cout << "InstrumentSelector " << ID << ": Record ON Press on block " << block << std::endl;
    }
    else if (  state->recEnable && stateStore->trackoutputState.at(ID).recEnable == true )
    {
      std::cout << "InstrumentSelector " << ID << ": Record OFF Press on block " << block << std::endl;
    }
    else
    {
      std::cout << "InstrumentSelector " << ID << ": Play Button Press on block " << block << std::endl;
    }
    
    // 
    EngineEvent* x = new EngineEvent();
    x->looperSelectBuffer(ID,block);
    top->toEngineQueue.push(x);
  }
  else if( event->type == GDK_BUTTON_PRESS && event->button == 3 ) // right
  {
    std::cout << "InstrumentSelector " << ID << ": Load event on block " << block << std::endl; 
    loadSample(block);
  }
  */
  
  return true;
}

void InstrumentSelector::dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time)
{
  cout << "InstrumentSelector::dragFunction() called!" << endl;
  
  // drag target is of string type, send
  cout << "DRAG from block " << block << endl;
  
  list<string>::iterator i = fileList.begin();
  
  for( int counter = 0; i != fileList.end(); i++ )
  {
    if ( counter == block )
    {
      // send data to drop
      std::string fullFilename = Glib::build_filename( currentDir, *i );
      selection_data.set( selection_data.get_target(), 8 /* 8 bits format */, (guchar*) fullFilename.c_str(), fullFilename.size() /* the length of I'm Data! in bytes */);
    }
    counter++;
  }
  
}







