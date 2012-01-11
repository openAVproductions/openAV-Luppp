
#include "g_lvtwodisplay.hpp"

#include "top.hpp"

int Lv2Display::privateID = 0;

Lv2Display::Lv2Display(Top* t, GuiStateStore* s)
{
  // Give each Lv2Display an ID
  ID = privateID++;
  
  top = t;
  
  stateStore = s;
  
  // init a Gtk::Window for showing this widget
  window = new Gtk::Window();
  
  // create a suil host
  suilHost = suil_host_new( writeFunc, indexFunc, subscribeFunc, unsubscribeFunc);
  
  widget = 0;
  
  loadPlugin("");
  
  signal_toggled().connect ( sigc::mem_fun ( *this, &Lv2Display::toggleWindow ) );
}

void Lv2Display::loadPlugin(std::string pluginUri)
{
  std::cout << "Plugin String = " << pluginUri << std::endl;
  //std::cout << "LOADPLUGIN: Comparing string TUBE: " << pluginUri.compare("http://invadarecords.com/plugins/lv2/tube/mono") << std::endl;
  //std::cout << "LOADPLUGIN: Comparing string Compressor: " << pluginUri.compare("http://invadarecords.com/plugins/lv2/compressor/mono") << std::endl;
  
  if (ID == 0 || pluginUri.compare("http://invadarecords.com/plugins/lv2/compressor/mono") == 0)
  {
    set_label( "Invada Comp" );
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
  else if (ID == 1 || pluginUri.compare( "http://invadarecords.com/plugins/lv2/tube/mono" ) == 0 )
  {
    label = "Invada Tube Distort";
    set_label( label );
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
    set_label( label );
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
    set_label( label );
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
    set_label( "Uncertified Plugin!" );
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
  
  if ( get_active() == true )
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

Lv2Display::~Lv2Display()
{
  std::cout << "Lv2Display::~Lv2Display()" << std::endl;
  suil_host_free (suilHost);
}
