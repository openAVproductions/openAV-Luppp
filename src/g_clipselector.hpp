
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
#ifndef LUPPP_CLIPSELECTOR
#define LUPPP_CLIPSELECTOR

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

#include "top.hpp"
#include "g_statestore.hpp"
#include "clipselectorstate.hpp"

class ClipSelector : public Gtk::DrawingArea
{
  public:
    ClipSelector(Top*,GuiStateStore*);
    ClipSelector(Top*,GuiStateStore*, bool); // masterClipSelector
    ~ClipSelector();
    
    bool redraw();
  
  protected:
    int ID;
    static int privateID;
    
    bool masterClipSelector;
    int  masterClipPlaying;
    int  masterClipQueued;
    
    // for drag block source
    int block;
    
    Top* top;
    GuiStateStore* stateStore;
    
    std::list<std::string> nameList;
    
    //Override default signal handler:
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    
    // drop functions
    void dropFunction( const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
                       const Gtk::SelectionData& selection_data, guint info, guint time);
    
    void dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
    
    // action functions
    void loadSample(int);
};

#endif // SCENE_SELECTOR
