
#include "g_buffersource.hpp"

#include "audiosource.hpp"
#include "offlineworker.hpp"

using namespace std;
using namespace Luppp;

std::string intToStr (int number)
{
     std::ostringstream buff;
     buff<<number;
     return buff.str();
}

int GBufferSource::privateID = 0;

GBufferSource::GBufferSource(Top* t, GuiStateStore* s)
{
  ID = privateID++;
  
  top = t;
  stateStore = s;
  
  //setBufferSource.set_label("Audio Buffer");
  setLv2Source.set_label("Make LV2 instrument");
  setFluidSynthSource.set_label("Make Soundfont player");
  setWhiteNoiseSource.set_label("Make White Noise Gen");
  
  pMenu.add( setLv2Source );
  pMenu.add( setFluidSynthSource );
  pMenu.add( setWhiteNoiseSource );
  pMenu.show_all();
  
  // connect popup menu items to functions
  setFluidSynthSource.signal_activate().connect( sigc::mem_fun(*this, &GBufferSource::fluidSynthSource));
  setWhiteNoiseSource.signal_activate().connect( sigc::mem_fun(*this, &GBufferSource::whiteNoiseSource));
  
  // Gives Button presses to the widget, and connect them to onMouseButtonDown
  add_events(Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &GBufferSource::on_button_press_event) );
  
  // set default widget size
  set_size_request(130,216);
}

void GBufferSource::fluidSynthSource()
{
  // so we ask the user to pick a soundfont, and then make OfflineWorker
  // load it and send it to Engine
  cout << "GUI got FluidSynthSource menu activation, sending EE_SET_TRACK_SOURCE event to Engine" << endl;
  
  // ask for soundfont path & filename here
  
  // ask for soundfont bank / patch number (/ name?) here
  
  // tell offlineWorker to do the loading
  top->offlineWorker->setTrackSource(ID, AUDIO_SOURCE_TYPE_FLUIDSYNTH);
}

void GBufferSource::whiteNoiseSource()
{
  top->offlineWorker->setTrackSource(ID, AUDIO_SOURCE_TYPE_WHITENOISE);
}

bool GBufferSource::redraw()
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

bool GBufferSource::on_button_press_event(GdkEventButton* event)
{
  
  std::cout << "Event Type: " << event->type << ". Event Button: " << event->button << "." << std::endl;
  
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    std::cout << "Event Type: " << event->type << ". Event Button: " << event->button << "." << std::endl;
    
    if(event->button == 3)
    {
      //ifdef DEBUG
      std::cout << "GBufferSource.cpp: Event RightClick on GBufferSource instance at " << this <<"." << std::endl;
      
      pMenu.popup(event->button, event->time);
    }
    else if (event->button == 1)
    {
      // normal click: trigger !currentRecordState
      //lo_send( lo_address_new( NULL,"14688") , "/luppp/track/record","ii",ID, !recording );
    }
    
    return true; //It's been handled.
  }
  else
  {
    std::cout << event -> type << std::endl;
  }
  
  return true;
}

bool GBufferSource::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  if(window)    // Only run if Window does exist
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    
    // set line Width
    cr -> set_line_width(6.0);
    cr -> set_line_join ( Cairo::LINE_JOIN_ROUND);
    cr -> set_line_cap  ( Cairo::LINE_CAP_ROUND );
    
    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    cr->rectangle(event->area.x, event->area.y,event->area.width, event->area.height);
    cr->clip();
    
    int x = 10;
    int y = 22;
    int xSize = 110;
    int ySize = 95;
    
    // get values from GuiStateStore
    float angle = stateStore->bufferAudioSourceState.at(ID).index;
    bool active = true;
    
    // background whole
    setColour(cr, COLOUR_GREY_3 );
    cr->rectangle(0, 0, 130, 216);
    cr->fill();
    
    // inner square
    setColour(cr, COLOUR_GREY_4 );
    cr->rectangle(x, y, xSize, ySize);
    cr->fill();
    
    float xc = x + (0.5*xSize);
    float yc = y + (ySize*0.5);
    
    // outer circle
    setColour(cr, COLOUR_GREY_3 );
    cr ->arc(xc, yc, 31, 0, 2 * 3.1415);
    cr ->fill_preserve();
    setColour(cr, COLOUR_GREY_4 );
    cr->set_line_width(4);
    cr ->stroke();
    
    cr->move_to( xc, yc );
    
    float tmpAngle = angle - ((3.1415*2) * (3.f/8.f));
    
    cr->arc(xc, yc, 30, (3.1415*2) * tmpAngle, (3.1415*2) * tmpAngle + 3.1415/2);
    cr->close_path();
    if ( active )
      setColour(cr, COLOUR_BLUE_1, 0.7 );
    else
      setColour(cr, COLOUR_GREY_2, 0.7 );
    cr->fill_preserve();
    if ( active )
      setColour(cr, COLOUR_ORANGE_1 );
    else
      setColour(cr, COLOUR_GREY_1, 0.7 );
    cr->set_line_width(3);
    cr->stroke();
    
    // outline
    setColour(cr, COLOUR_GREY_2 );
    cr->rectangle( x, y , xSize, ySize );
    cr->set_line_width(4);
    cr->stroke();
    
    TitleBar(cr, 0,0, 130, 216, "Buffer Looper", active);
    
    /*
    if ( selected )
    {
      cr->rectangle(x, y, xSize, ySize);
      setColour( cr, COLOUR_ORANGE_1 );
      cr->set_line_width(1);
      cr->stroke();
    }
    */
    
  } 
  return true;
  
} // on_expose_event()

GBufferSource::~GBufferSource()
{
}
