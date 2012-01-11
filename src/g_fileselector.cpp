
#include "g_fileselector.hpp"

#include "g_widgets.hpp"

#include <glibmm/fileutils.h>
//#include <gtkmm/filechooserdialog.h>

using namespace std;
using namespace Luppp;

FileSelector::FileSelector(Top* t, GuiStateStore* s)
{
  top = t;
  stateStore = s;
  
  //std::cout << "Enterin FileSelector contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &FileSelector::on_button_press_event) );
  
  // drag target types
  std::list<Gtk::TargetEntry> listTargets;
  listTargets.push_back( Gtk::TargetEntry("AUDIO_FILE_STRING") );
  
  // drag source
  drag_source_set(listTargets);
  
  // drag function connect
  signal_drag_data_get().connect(sigc::mem_fun(*this, &FileSelector::dragFunction ));
  
  widgetSizeX = 200;
}

bool FileSelector::redraw()
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

void FileSelector::setWidgetSizeFromList()
{
  
  int widgetY = 18 * fileList.size();
  cout << "FileSelector::setWidgetSizeFromList() setting widget size to " << widgetY << endl;
  set_size_request(200, widgetY );
}


bool FileSelector::on_expose_event(GdkEventExpose* event)
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
    
    // update contents of lists only when dir changed
    if ( currentDir != stateStore->getLastDir() )
    {
      currentDir = stateStore->getLastDir();
      Glib::Dir dir ( currentDir );
      list<string> tmpList(dir.begin(), dir.end());
      
      // empty current contents
      fileList.clear();
      
      std::list<std::string>::iterator i = tmpList.begin();
      for(; i != tmpList.end(); i++ )
      {
        int found = i->find(".wav");
        
        if (found != string::npos )
        {
          cout << "found wav @ " << found << ", contents =  " << *i << endl;
          // add the filename to the .wav fileList
          fileList.push_back( *i );
        }
      }
      // finally resize widget based on new list size
      setWidgetSizeFromList();
    }
    
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

void FileSelector::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void FileSelector::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool FileSelector::on_button_press_event(GdkEventButton* event)
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
      std::cout << "FileSelector " << ID << ": Record ON Press on block " << block << std::endl;
    }
    else if (  state->recEnable && stateStore->trackoutputState.at(ID).recEnable == true )
    {
      std::cout << "FileSelector " << ID << ": Record OFF Press on block " << block << std::endl;
    }
    else
    {
      std::cout << "FileSelector " << ID << ": Play Button Press on block " << block << std::endl;
    }
    
    // 
    EngineEvent* x = new EngineEvent();
    x->looperSelectBuffer(ID,block);
    top->toEngineQueue.push(x);
  }
  else if( event->type == GDK_BUTTON_PRESS && event->button == 3 ) // right
  {
    std::cout << "FileSelector " << ID << ": Load event on block " << block << std::endl; 
    loadSample(block);
  }
  */
  
  return true;
}

void FileSelector::dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time)
{
  cout << "FileSelector::dragFunction() called!" << endl;
  
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







