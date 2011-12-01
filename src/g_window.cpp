
#include "g_window.hpp"

using namespace std;

Window::Window(Gtk::Main *k, Top* t)
{
  // store the "kit" instance from main to run it at the end of the
  // constructor.
  kit = k;
  
  
  // We take a Top* and keep it local (its static so this is safe).
  // Connecting the "dispatcher" signal to the "handleEvent()" function
  // of the StateStore performs the functions on the store, and then
  // we call "redraw()" on the nessiary widgets to refresh the GUI.
  top = t;
  
  top->guiDispatcher->connect( sigc::mem_fun( this, &Window::handleEvent) );
  
  
  
  // load Glade file
  Glib::RefPtr<Gtk::Builder> refBuilder;
  
  try {
    refBuilder = Gtk::Builder::create_from_file("ui.glade");
  }
  catch (Gtk::BuilderError e) {
    std::cout << "Gtk::BuilderError loading mainwindow glade file: " << e.what() << std::endl;
    exit(1);
  }
  catch (Glib::FileError e) {
    std::cout << "Gtk::FileError loading mainwindow glade file: " << e.what() << std::endl;
    exit(1);
  }
  
  // get the widgets
  refBuilder->get_widget("window", window);
  window->set_title("Luppp 2.0");
  
  refBuilder->get_widget("mainTable", mainTable);
  
  addTrack();
  
  // last thing, now we're starting the GUI main loop
  kit->run(*window);
}

void Window::handleEvent()
{
  cout << "Window::handleEvent()" << endl;
  EngineEvent* e = top->toGuiQueue.pull();
  
  // replace this style code with code in StateStore, and have stateStore
  // update its GUI componenets based on values changed
  if ( e->type == EE_MIXER_VOLUME )
  {
    cout << "MixerVolume" << e->ia << ", " << e->fa << endl;
    list<TrackOutputState>::iterator i = guiState.trackoutputState.begin();
    advance(i,e->ia);
    i->volume = e->fa;
    
    // manually update GUI
    std::list<TrackOutput*>::iterator iter = trackoutputList.begin();
    (*iter)->redraw();
  }
}

void Window::addTrack()
{
  std::cout << "Window::addTrack()" << std::endl;
  trackoutputList.push_back( new TrackOutput( &guiState ) );
  
  std::list<TrackOutput*>::iterator i = trackoutputList.begin();
  
  mainTable->attach( **i, 0, 1, 7, 8);
  mainTable->show_all();
}

