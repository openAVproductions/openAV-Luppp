
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

#ifndef LUPPP_LVTWODISPLAY
#define LUPPP_LVTWODISPLAY

#include "engineevent.hpp"
#include "g_widgetbase.hpp"

#include "g_statestore.hpp"

#include <iostream>
#include <sstream>
#include <gtkmm/drawingarea.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm.h>

#include <suil/suil.h>

class Top;

class Lv2Display : public Gtk::DrawingArea, public WidgetBase
{
  public:
    Lv2Display(Top*, GuiStateStore*);
    ~Lv2Display();
    void portEvent(EngineEvent* ev);
    void loadPlugin(std::string);
    
    bool redraw();
  
  protected:
    int ID;
    static int privateID;
    
    Top* top;
    GuiStateStore* stateStore;
    
    Gtk::Window   *window;
    GtkWidget     *widget;
    
    bool active;
    bool showUI;
    
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
    
    // gtkmm drawingarea functions
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
};

#endif // CSOUND_DISPLAY
