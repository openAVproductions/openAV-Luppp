
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
  
  // initialize variables
  numTracks = 0;
  
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
  addTrack();
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
    cout << "MixerVolume: " << e->ia << ", " << e->fa << endl;
    list<TrackOutputState>::iterator i = guiState.trackoutputState.begin();
    advance(i,e->ia);
    i->volume = e->fa;
    
    // manually update GUI
    std::list<TrackOutput*>::iterator iter = trackoutputList.begin();
    advance(iter,e->ia);
    (*iter)->redraw();
    
    std::list<Gtk::ProgressBar*>::iterator progIter = trackprogressList.begin();
    advance(progIter,e->ia);
    (*progIter)->set_fraction(e->fa);
  }
}

void Window::addTrack()
{
  std::cout << "Window::addTrack()" << std::endl;
  
  std::stringstream trackName;
  trackName << numTracks;
  
  // label
  tracklabelList.push_back( new Gtk::Label( trackName.str() ) );
  std::list<Gtk::Label*>::iterator labelIter = tracklabelList.begin();
  std::advance(labelIter,numTracks);
  mainTable->attach( **labelIter, numTracks, numTracks+1, 0, 1);
  
  // input selector
  trackinputList.push_back( new Gtk::ComboBoxText() );
  std::list<Gtk::ComboBoxText*>::iterator inputIter = trackinputList.begin();
  std::advance(inputIter,numTracks);
  (**inputIter).append_text("test");
  (**inputIter).set_active(0);
  mainTable->attach( **inputIter, numTracks, numTracks+1, 1, 2);
  
  // progress bar
  trackprogressList.push_back( new Gtk::ProgressBar() );
  std::list<Gtk::ProgressBar*>::iterator progressIter = trackprogressList.begin();
  std::advance(progressIter,numTracks);
  (**progressIter).set_fraction(numTracks / 8.f);
  mainTable->attach( **progressIter, numTracks, numTracks+1, 3, 4);
  
  // fader / pan
  trackoutputList.push_back( new TrackOutput( &guiState ) );
  std::list<TrackOutput*>::iterator i = trackoutputList.begin();
  std::advance(i,numTracks);
  mainTable->attach( **i, numTracks, numTracks+1, 7, 8);
  
  mainTable->show_all();
  numTracks++;
}

