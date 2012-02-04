
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

#include "g_clipselector.hpp"

#include "g_widgets.hpp"

#include <sstream>

#include "gtkmm.h"
#include "gtkmm/filechooserdialog.h"

#include "offlineworker.hpp"

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
  
  // drop target types
  std::list<Gtk::TargetEntry> dropTargets;
  dropTargets.push_back( Gtk::TargetEntry("AUDIO_FILE_STRING") );
  
  // make drop destination
  drag_dest_set(dropTargets);
  
  // connect drop signal
  signal_drag_data_received().connect(sigc::mem_fun(*this, &ClipSelector::dropFunction) );
  
  // drag buffer UID out of clipselector
  std::list<Gtk::TargetEntry> dragTargets;
  dragTargets.push_back( Gtk::TargetEntry("AUDIO_BUFFER_ID") );
  
  // drag source
  drag_source_set(dragTargets);
  
  // drag function connect
  signal_drag_data_get().connect(sigc::mem_fun(*this, &ClipSelector::dragFunction )); 
  
  
  masterClipSelector = false;
  masterClipPlaying  = -1;
  masterClipQueued   = -1;
  
  set_size_request(74,18 * 10);
}

ClipSelector::ClipSelector(Top* t, GuiStateStore* s, bool masterConstructor)
{
  cout << "ClipSelector MASTER constructor called!" << endl;
  top = t;
  stateStore = s;
  
  masterClipSelector = true;
  
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &ClipSelector::on_button_press_event) );
  
  ID = -1;
  
  for ( int i = 0; i < 10; i++ )
  {
    stringstream s;
    s << i;
    nameList.push_back( s.str() );
  }
  set_size_request(74,18 * 10);
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
    if ( masterClipSelector )
      setColour(cr, COLOUR_GREY_4);
    else
      setColour(cr, COLOUR_GREY_3);
    cr->fill();
    
    ClipSelectorState* state;
    
    float y = 0.f;
    
    if ( masterClipSelector )
    {
      state = new ClipSelectorState;
      y += 2; // align master track due to emtpy VBox for input selector in tracks
    }
    else
    {
      state = &stateStore->clipSelectorState.at(ID);
    }
    
    //std::cout << "Redrawing GCLipSelector " << ID << " now...  state.recording = " << stateStore->clipSelectorState.at(ID).recording << std::endl;
    //std::cout << "\t\t CHECK:     stateStore.clipSelectorState.at(e->ia).recording = " << stateStore->clipSelectorState.at(ID).recording << std::endl;
    
    // 5 should be replaced with "max num scenes" from Store
    for( int i = 0; i < 10; i++)
    {
      // prepare values
      std::list<ClipInfo>::iterator iter = state->clipInfo.begin();
      std::advance(iter, i);
      
      ClipState clipState = CLIP_STATE_EMPTY;
      
      if ( iter->hasBuffer )
      {
        clipState = CLIP_STATE_LOADED;
        
        if ( state->playing == i ) // if also playing, make green
          clipState = CLIP_STATE_PLAYING;
      }
      
      if ( state->recording == i )
      {
        cout << "Setting a RECORDING BLOCK now!! " << endl;
        clipState = CLIP_STATE_RECORDING;
      }
      
      std::string name = "";
      
      if ( (*iter).bufferID < stateStore->audioBufferNameVector.size() )
        name = stateStore->audioBufferNameVector.at( (*iter).bufferID );
      else
        name = "inv";
      
      cout << "Clip " << (*iter).bufferID << " has name " << name << endl;
      
      // retrieve audio buffer name from unique bufferID and audioBufferNameVector
      if ( (*iter).bufferID < stateStore->audioBufferNameVector.size() )
      {
        name = stateStore->audioBufferNameVector.at( (*iter).bufferID );
        cout << "AudioClip @ " << (*iter).bufferID << " got name " << name << endl;
      }
      
      if ( masterClipSelector )
      {
        list<string>::iterator iter = nameList.begin();
        advance(iter, i);
        name = *iter;
        clipState = CLIP_STATE_MASTER_TRACK;
        if ( i == masterClipPlaying )
        {
          clipState = CLIP_STATE_PLAYING;
        }
      }
      
      cr->rectangle(0,y,event->area.width,17);
      
      Block(cr, 0, y, clipState, name );
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
  block = ((int)event->y) / 18;
  
  if ( masterClipSelector )
  {
    if ( event->button == 1 )
    {
      // send clip activate on all tracks ( counted trough hack on static int) and return
      for ( int i = 0; i < privateID; i++ )
      {
        EngineEvent* x = new EngineEvent();
        x->looperSelectBuffer(i,block);
        top->toEngineQueue.push(x);
      }
      masterClipPlaying = block;
      redraw();
    }
    else // rename scene
    {
      Gtk::Dialog dialog("Rename Scene", false); // not modal, leave program update!
      
      Gtk::Entry* entry = new Gtk::Entry();
      entry->set_activates_default (true);
      
      dialog.add_button( Gtk::Stock::OK, Gtk::RESPONSE_OK );
      dialog.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
      
      dialog.set_default_response( Gtk::RESPONSE_OK );
      dialog.get_vbox()->add( *entry );
      dialog.get_vbox()->show_all();
      
      list<string>::iterator iter = nameList.begin();
      string name;
      
      switch ( dialog.run() )
      {
        case Gtk::RESPONSE_OK:
          name = entry->get_buffer()->get_text();
          cout << " Dialog OK clicked, entry contains: " << name << endl;
          advance(iter, block);
          *iter = name;
          redraw();
          break;
        default: break;
      }
      
    }
    
    return true;
  }
  
  if( event->type == GDK_BUTTON_PRESS && event->button == 1 ) // left
  {
    // so we're gonna play a clip, if the track is record enabled, and the
    // current block is *NOT* recording yet, we set it to record, otherwise
    // we stop it from recording. If the track isn't record armed: then we
    // send a play message
    TrackOutputState* state = &stateStore->trackoutputState.at(ID);
    if ( state->recEnable && stateStore->trackoutputState.at(ID).recEnable == false )
    {
      std::cout << "ClipSelector " << ID << ": Record ON Press on block " << block << std::endl;
    }
    else if (  state->recEnable && stateStore->trackoutputState.at(ID).recEnable == true )
    {
      std::cout << "ClipSelector " << ID << ": Record OFF Press on block " << block << std::endl;
    }
    else
    {
      std::cout << "ClipSelector " << ID << ": Play Button Press on block " << block << std::endl;
    }
    
    // 
    EngineEvent* x = new EngineEvent();
    x->looperSelectBuffer(ID,block);
    top->toEngineQueue.push(x);
  }
  else if( event->type == GDK_BUTTON_PRESS && event->button == 3 ) // right
  {
    std::cout << "ClipSelector " << ID << ": Load event on block " << block << std::endl; 
    loadSample(block);
  }
  
  return true;
}

void ClipSelector::dropFunction( const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
                       const Gtk::SelectionData& selection_data, guint info, guint time)
{
  // drop target is of string type, load that sample
  
  int block = y / 18;
  cout << "DROP on block " << block << endl;
  
  const int length = selection_data.get_length();
  if( (length >= 0) ) // && (selection_data.get_format() == 8)
  {
    std::string filename = selection_data.get_data_as_string();
    std::cout << "Received " << filename << " in ClipSelector, loading now! " << std::endl;
    
    // audioFileLoader informs engine & updates StateStore
    int ret = top->offlineWorker->loadAudioBuffer( ID, block, filename );
    
    if ( ret == 0 ) // successful load, so store filename in vector
    {
      stateStore->audioBufferNameVector.push_back( Glib::path_get_basename(filename) );
    }
  }

  context->drag_finish(false, false, time);
}


void ClipSelector::dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time)
{
  cout << "ClipSelector::dragFunction() called!" << endl;
  
  // drag target is of string type, send
  cout << "DRAG from block " << block << endl;
  
  // get state of clip selector
  ClipSelectorState* state = &stateStore->clipSelectorState.at(ID);
  
  // get block info, to retrieve its bufferID
  std::list<ClipInfo>::iterator iter = state->clipInfo.begin();
  std::advance(iter, block);
  
  stringstream s;
  s << (*iter).bufferID;
  
  // send data to drop
  selection_data.set( selection_data.get_target(), 8, (guchar*) s.str().c_str(), s.str().size() );
  
}


// loadsample has access to ID, so we know the track already
void ClipSelector::loadSample(int block)
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
      
      // audioFileLoader informs engine & updates StateStore
      int ret = top->offlineWorker->loadAudioBuffer( ID, block, filename );
      
      if ( ret == 0 ) // successful load, so store filename in vector
      {
        stateStore->audioBufferNameVector.push_back( Glib::path_get_basename(filename) );
      }
      
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
