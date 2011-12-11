
#include "g_clipselector.hpp"

#include "g_widgets.hpp"

#include "gtkmm/filechooserdialog.h"

using namespace std;
using namespace Luppp;

int ClipSelector::privateID = 0;

ClipSelector::ClipSelector(Top* t, GuiStateStore* s)
{
  ID = privateID++;
  
  top = t;
  stateStore = s;
  
  //std::cout << "Enterin ClipSelector contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &ClipSelector::on_button_press_event) );
  
  set_size_request(74,18 * 5);
}

bool ClipSelector::redraw()
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


bool ClipSelector::on_expose_event(GdkEventExpose* event)
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
    
    std::cout << "StateStore = " << &stateStore << std::endl;
    
    ClipSelectorState state = stateStore->clipSelectorState.at(ID);
    
    float y = 0.f;
    
    ClipState clipState;
    
    
    // 5 should be replaced with "max num scenes" from Store
    for( int i = 0; i < 5; i++)
    {
      clipState = state.clipStates.at(i);
      
      // void Block(Cairo::RefPtr<Cairo::Context> cr, float x, float y, bool active);
      Block(cr, 0, y, clipState);
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

void ClipSelector::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void ClipSelector::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool ClipSelector::on_button_press_event(GdkEventButton* event)
{
  int block = ((int)event->y) / 18;
  
  if( event->type == GDK_BUTTON_PRESS && event->button == 1 ) // left
  {
    std::cout << "ClipSelector: Button Press on block " << block << std::endl; 
  }
  else if( event->type == GDK_BUTTON_PRESS && event->button == 3 ) // right
  {
    std::cout << "ClipSelector: Load event on block " << block << std::endl; 
    loadSample();
  }
  
  return true;
}

void ClipSelector::loadSample()
{
  std::cout << "ClipSelector::loadSample()" << std::endl;
  int result = 0;
  std::string filename;
  
  Gtk::FileChooserDialog dialog("Load Sample");
  
  dialog.set_current_folder( stateStore->getLastDir() );
  
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Load", Gtk::RESPONSE_OK);
  
  result = dialog.run();
  filename = dialog.get_filename();
  
  //Handle the response:
  switch(result)
  {
    case(Gtk::RESPONSE_OK):
    {
      stateStore->setLastDir( Glib::path_get_dirname(filename) );
      
      std::cout << "Lo" << std::endl;
      EngineEvent* x = new EngineEvent();
      TrackOutputState* state = &stateStore->trackoutputState.at(ID);
      x->setTrackSolo(ID, !state->solo);
      top->toEngineQueue.push(x);
      
      return;
    }
    case(Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Cancel clicked." << std::endl;
      return;
    }
    default:
    {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

ClipSelector::~ClipSelector()
{
}
