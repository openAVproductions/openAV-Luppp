
#ifndef LUPPP_LVTWODISPLAY
#define LUPPP_LVTWODISPLAY

#include "engineevent.hpp"

#include "g_statestore.hpp"

#include <iostream>
#include <sstream>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm.h>

#include <suil/suil.h>

class Top;

class Lv2Display : public Gtk::ToggleButton
{
  public:
    Lv2Display(Top*, GuiStateStore*);
    ~Lv2Display();
    void portEvent(EngineEvent* ev);
    void loadPlugin(std::string);
  
  protected:
    int ID;
    static int privateID;
    
    Top* top;
    GuiStateStore* stateStore;
    
    Gtk::Window   *window;
    GtkWidget     *widget;
    
    void toggleWindow();
    
    // Suil variables
    SuilHost      *suilHost;
    SuilInstance  *instance;
    SuilWidget    *suilWidget;
    std::string   label;
    
    std::vector<float> portValueArray;
    std::vector<LV2_Feature*> featureArray;
    
    // Suil functions
    static void writeFunc(SuilController controller, uint32_t port_index, uint32_t buffer_size, uint32_t protocol, void const* buffer);
    static unsigned int indexFunc(SuilController controller, const char*    port_symbol);
    static unsigned int subscribeFunc(SuilController controller,uint32_t port_index,uint32_t protocol,const LV2_Feature* const* features);
    static unsigned int unsubscribeFunc(SuilController controller,uint32_t port_index,uint32_t protocol,const LV2_Feature* const* features);
    
};

#endif // CSOUND_DISPLAY
