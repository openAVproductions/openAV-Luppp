
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

#include "g_lvtwodisplay.hpp"

#include "top.hpp"

#include "g_widgets.hpp"

using namespace Luppp;

int Lv2Display::privateID = 0;

Lv2Display::Lv2Display(Top* t, GuiStateStore* s)
{
  ID = ID = WidgetBase::getID();
  
  top = t;
  
  stateStore = s;
  
  // init a Gtk::Window for showing this widget
  window = new Gtk::Window();
  
  // create a suil host
  suilHost = suil_host_new( writeFunc, indexFunc, subscribeFunc, unsubscribeFunc);
  
  widget = 0;
  
  loadPlugin("http://invadarecords.com/plugins/lv2/phaser/mono");
  
  active = false;
  
  showUI = false;
  
  // for this drawingarea widget
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK);
  set_size_request(130, 216);
}

void Lv2Display::loadPlugin(std::string pluginUri)
{
  std::cout << "Plugin String = " << pluginUri << std::endl;
  //std::cout << "LOADPLUGIN: Comparing string TUBE: " << pluginUri.compare("http://invadarecords.com/plugins/lv2/tube/mono") << std::endl;
  //std::cout << "LOADPLUGIN: Comparing string Compressor: " << pluginUri.compare("http://invadarecords.com/plugins/lv2/compressor/mono") << std::endl;
  
  if (ID == 0 || pluginUri.compare("http://invadarecords.com/plugins/lv2/compressor/mono") == 0)
  {
    label = "Invada Comp";
    instance = suil_instance_new(  suilHost,
                    (void*)&ID, // pass a pointer to the controller objects here
                    
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // target UI widget type
                    "http://invadarecords.com/plugins/lv2/compressor/mono", // plugin URI
                    "http://invadarecords.com/plugins/lv2/compressor/gui",  // plugin UI URI
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // plugin UI TYPE URI
                    "/usr/lib/lv2/invada.lv2/",                             // plugin UI bundle
                    "/usr/lib/lv2/invada.lv2/inv_compressor_gui.so",        // plugin UI binary
                    &featureArray[0]);                                      // supported features by host
  }
  else if (ID == 1 || pluginUri.compare( "http://invadarecords.com/plugins/lv2/phaser/mono" ) == 0 )
  {
    label = "Invada Phaser";
    instance = suil_instance_new(  suilHost,
                    (void*)&ID, // pass a pointer to the controller objects here
                    
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // target UI widget type
                    "http://invadarecords.com/plugins/lv2/phaser/mono", // plugin URI
                    "http://invadarecords.com/plugins/lv2/phaser/gui",  // plugin UI URI
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // plugin UI TYPE URI
                    "/usr/lib/lv2/invada.lv2/",                             // plugin UI bundle
                    "/usr/lib/lv2/invada.lv2/inv_phaser_gui.so",        // plugin UI binary
                    &featureArray[0]);                                      // supported features by host
  }
  else if (ID == 1 || pluginUri.compare( "http://invadarecords.com/plugins/lv2/tube/mono" ) == 0 )
  {
    label = "Invada Tube Distort";
    //set_label( label );
    instance = suil_instance_new(  suilHost,
                    (void*)&ID, // pass a pointer to the controller objects here
                    
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // target UI widget type
                    "http://invadarecords.com/plugins/lv2/tube/mono",       // plugin URI
                    "http://invadarecords.com/plugins/lv2/tube/gui",        // plugin UI URI
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // plugin UI TYPE URI
                    "/usr/lib/lv2/invada.lv2/",                             // plugin UI bundle
                    "/usr/lib/lv2/invada.lv2/inv_tube_gui.so",                      // plugin UI binary 
                    &featureArray[0]);                                      // supported features by host

  }
  else if (ID == 3 || pluginUri.compare("http://invadarecords.com/plugins/lv2/erreverb/mono") == 0 )
  {
    label = "Invada Reverb";
    //set_label( label );
    instance = suil_instance_new(  suilHost,
                    (void*)&ID, // pass a pointer to the controller objects here
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // target UI widget type
                    "http://invadarecords.com/plugins/lv2/erreverb/mono",   // plugin URI
                    "http://invadarecords.com/plugins/lv2/erreverb/gui",    // plugin UI URI
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // plugin UI TYPE URI
                    "/usr/lib/lv2/invada.lv2/",                             // plugin UI bundle
                    "/usr/lib/lv2/invada.lv2/inv_erreverb_gui.so",          // plugin UI binary 
                    &featureArray[0]);                                      // supported features by host
  }
  else if (ID == 2 )
  {
    label = "AMS VCO2";
    //set_label( label );
    instance = suil_instance_new(  suilHost,
                    (void*)&ID, // pass a pointer to the controller objects here
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // target UI widget type
                    "http://lv2plug.in/plugins/avw/vco2",                   // plugin URI
                    "http://lv2plug.in/plugins/avw/vco2/gui",               // plugin UI URI
                    "http://lv2plug.in/ns/extensions/ui#GtkUI",             // plugin UI TYPE URI
                    "/usr/local/lib/lv2/avw.lv2/",                   // plugin UI bundle
                    "/usr/local/lib/lv2/avw.lv2/vco2_gui.so",        // plugin UI binary 
                    &featureArray[0]);                                      // supported features by host
  }
  else
  {
    // uncertified plugin loaded, return with a warning!
    std::cout << "Lv2Display::loadPlugin() WARNING! Unknown plugin loaded into engine!" << std::endl;
    //set_label( "Uncertified Plugin!" );
    label = "Uncertified Plugin!";
    return;
  }
  
  
  // Cast the suilwidget to a Gtk::Widget, and store it in this class
  SuilWidget suilWidget = suil_instance_get_widget(instance);
  widget = (GtkWidget*) suilWidget;
  gtk_container_add (GTK_CONTAINER ( window->gobj() ), widget);
}

void Lv2Display::toggleWindow()
{
  if (widget == 0)
  {
    return;
  }
  
  // toggle the value, then update
  showUI = !showUI;
  
  if ( showUI == true )
  {
    window->show_all();
  }
  else
  {
    window->hide();
  }
}

void Lv2Display::writeFunc(SuilController controller, uint32_t port_index, uint32_t buffer_size, uint32_t protocol, void const* buffer)
{
  //std::cout << "Lv2Display::writeFunc() called!" << std::endl;
  
  int tmpID = *(int*)controller;
  
  if ( protocol != 0 )
  {
    std::cout << "Lv2Display, write() protocol != 0, returning!" << std::endl;
    return;
  }
  else if ( sizeof(buffer_size) != 4 )
  {
    std::cout << "Lv2Display, write() buffer_size != 4, returning!" << std::endl;
    return;
  }
  
  // data sent over OSC
  //lo_send( lo_address_new( NULL,"14688") , "/luppp/track/setpluginparameterabsolute", "iiif", -2, tmpID, port_index, *(float*)buffer );
  
  
  
  return;
}

unsigned int Lv2Display::indexFunc(SuilController controller, const char*    port_symbol)
{
  std::cout << "Lv2Display::indexFunc() called!" << std::endl;
  return 0;
}

unsigned int Lv2Display::subscribeFunc(SuilController controller,uint32_t port_index,uint32_t protocol,const LV2_Feature* const* features)
{
  std::cout << "Lv2Display::subscribeFunc() called!" << std::endl;
  
  // get a useable (in static func) rh*
  //ResourceHolder* tmpRh = (ResourceHolder*)controller;
  
  //Mixer* mixer = (Mixer*)tmpRh->mixer;
  //mixer->lv2hostArray.at(1)->subscribeFunc( port_index, float( *(float*)buffer )  );
  
  return 0;
}

unsigned int Lv2Display::unsubscribeFunc(SuilController controller,uint32_t port_index,uint32_t protocol,const LV2_Feature* const* features)
{
  std::cout << "Lv2Display::unsubscribeFunc() called!" << std::endl;
  return 0;
}

void Lv2Display::portEvent(EngineEvent* ev)
{
  if(widget == 0)
  {
    return;
  }

  suil_instance_port_event (  instance,
                ev->ic,
                4,
                0,
                (void*) &ev->fa ); 
    
    //std::cout << "Writing Lv2 event done!"<<std::endl;
  
}

bool Lv2Display::on_expose_event (GdkEventExpose* event)
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
    setColour(cr, COLOUR_GREY_3 );
    cr->fill();
    
    int x = 10;
    int y = 22;
    int xSize = 110;
    int ySize = 95;
    
    cr->rectangle(x, y, xSize, ySize);
    if ( showUI )
      setColour(cr, COLOUR_GREEN_1 );
    else
      setColour(cr, COLOUR_GREY_3 );
    cr->stroke();
    
    // id text
    cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->set_font_size ( 13 );
    cr->move_to ( x + 8, y + 17 );
    cr->set_source_rgb( 0 / 255.f, 0/255.f , 0/255.f );
    
    cr->show_text ( "Show UI" );
    
    TitleBar(cr, 0,0 , 250, 216, label, active);
    
  }
  
  return true;
}


bool Lv2Display::on_button_press_event(GdkEventButton* event)
{
  if( event->type == GDK_BUTTON_PRESS  ) // && event->button == 3
  {
    int x = 10;
    int y = 22;
    int xSize = 110;
    int ySize = 95;
    
    // graph area
    if ( (event->x > 10) && (event->x < 235) &&
         (event->y > 22) && (event->y < 117 ) )
    {
      /*
      std::cout << "graph area click!" << std::endl;
      mouseDown = true; // for pointer motion "drag" operations
      
      int evX = event->x;
      // inform engine of "click" and position co-efficents as such
      if ( evX < 50) evX = 50;
      if ( evX > 216)evX = 216;
      
      stateStore->cutoff = evX / float(xSize);
      cutoff = stateStore->cutoff;
      EngineEvent* x = new EngineEvent();
      x->setPluginParameter(0,0,0, cutoff );
      top->toEngineQueue.push(x);
      
      int evY = event->y;
      if (evY < 35 ) evY = 35;
      if (evY > 103) evY = 103;
      
      q = evY / float(ySize);
      x = new EngineEvent();
      x->setPluginParameter(0,0,1, q );
      top->toEngineQueue.push(x);
      */
    }
    
    if ( event->y < 20 )
    {
      std::cout << "GCompressor Enable / Disable click event!" << std::endl;
      
      /*
      EngineEvent* x = new EngineEvent();
      x->setTrackDeviceActive(ID, !stateStore->effectState.at(ID).active );
      top->toEngineQueue.push(x);
      */
      active = !active;
      redraw();
    }
    else
    {
      toggleWindow();
      redraw();
    }
    
    return true; //It's been handled.
  }
  else
    return false;
}


bool Lv2Display::redraw()
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


Lv2Display::~Lv2Display()
{
  std::cout << "Lv2Display::~Lv2Display()" << std::endl;
  suil_host_free (suilHost);
}
