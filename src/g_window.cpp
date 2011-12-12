
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
  
  
  // connect here if we want to provide "events" from engine, and only update
  // the GUI when the emit() is triggered, polling might serve better..?
  //top->guiDispatcher->connect( sigc::mem_fun( this, &Window::handleEvent) );
  
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
  refBuilder->get_widget("trackEffectBox", trackEffectBox);
  
  equalizer = new Equalizer( &guiState);
  
  // hack to show parametric
  trackEffectBox->add( *equalizer );
  trackEffectBox->show_all();
  
  addTrack();
  addTrack();
  //addTrack();
  
  // poll the event Queue
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Window::handleEvent), 50);
  
  // last thing, now we're starting the GUI main loop
  kit->run(*window);
}

int Window::handleEvent()
{
  //cout << "Window::handleEvent()" << endl;
  
  bool moreEventsWaiting = true;
  
  // loop over events queue, return when no events to process
  while ( moreEventsWaiting )
  {
    EngineEvent* e = top->toGuiQueue.pull();
    
    if ( e == 0 )
    {
      // exit the loop
      break;
    }
    
    //std::cout << "Non null event in GUI thread" << std::endl;
    
    // replace this style code with code in StateStore, and have stateStore
    // update its GUI componenets based on values changed
    if ( e->type == EE_MIXER_VOLUME )
    {
      if ( e->ia >= numTracks ) { cout << "MIXER_VOLUME: Out of bounds" << endl; return true; }
      //cout << "MixerVolume: " << e->ia << ", " << e->fa << endl;
      guiState.trackoutputState.at(e->ia).volume = e->fa;
      
      // manually update GUI
      std::list<TrackOutput*>::iterator iter = trackoutputList.begin();
      advance(iter,e->ia);
      //std::cout << "Redraw Iter = " << &iter << std::endl;
      (*iter)->redraw();
    }
    else if ( e->type == EE_TRACK_SET_MUTE ) {
      //std::cout << "GUI MUTE event: " << e->ib << std::endl;
      guiState.trackoutputState.at(e->ia).mute = e->ib;
      
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
    }
    else if ( e->type == EE_TRACK_SET_SOLO ) {
      guiState.trackoutputState.at(e->ia).solo = e->ib;
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
    }
    else if ( e->type == EE_TRACK_SET_REC ) {
      guiState.trackoutputState.at(e->ia).rec = e->ib;
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
    }
    else if ( e->type == EE_TRACK_SET_PAN ) {
      std::cout << "Gui got pan " << e->fa << std::endl; 
      guiState.trackoutputState.at(e->ia).pan = e->fa;
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
    }
    else if ( e->type == EE_TRACK_SET_SPEED )
    {
      guiState.fileAudioSourceState.at(e->ia).speed = e->fa;
      
      std::list<Gtk::ProgressBar*>::iterator progIter = trackprogressList.begin();
      advance(progIter,e->ia);
      (*progIter)->set_fraction(e->fa);
    }
    else if ( e->type == EE_LOOPER_LOAD )
    {
      cout << "LOOPER_LOAD event in GUI" << endl;
      if ( e->ia < guiState.clipSelectorState.size() ) {
        guiState.clipSelectorState.at(e->ia).clipInfo.at(e->ib).state = CLIP_STATE_LOADED;
        guiState.clipSelectorState.at(e->ia).clipInfo.at(e->ib).bufferID = e->ic;
        std::list<ClipSelector*>::iterator clipIter = clipselectorList.begin();
        advance(clipIter,e->ia);
        (*clipIter)->redraw();
      }
      else
        cout << "GUI: LOOPER_LOAD track OOB " << e->ia << endl;
    }
    else if ( e->type == EE_LOOPER_SELECT_BUFFER )
    {
      if ( e->ia < guiState.clipSelectorState.size() ) {
        // set previous playing to just "loaded"
        int playing = guiState.clipSelectorState.at(e->ia).playing;
        if ( playing >= 0 ) // check array access here
          guiState.clipSelectorState.at(e->ia).clipInfo.at(playing).state = CLIP_STATE_LOADED;
        
        // playing can = -1 for "nothing playing", so set regardless
        guiState.clipSelectorState.at(e->ia).playing = e->ib;
        
        // update currently playing, and set item to playing state
        if ( e->ib >= 0 ) // range check
        {
          guiState.clipSelectorState.at(e->ia).clipInfo.at(e->ib).state = CLIP_STATE_PLAYING;
        }
        else
          cout << "GUI: LOOPER_SELECT_BUFFER block OOB " << e->ib << endl;
        
        std::list<ClipSelector*>::iterator clipIter = clipselectorList.begin();
        advance(clipIter,e->ia);
        (*clipIter)->redraw();
      }
      else
        cout << "GUI: LOOPER_SELECT_BUFFER track OOB " << e->ia << endl;
    }
  }
  return true;
}

void Window::setEffectsBox(int trackID)
{
  // check its different, big redraw() call made for no reason otherwise
  if ( currentEffectsTrack == trackID ) {
    return;
  }
  
  currentEffectsTrack = trackID;
}

void Window::addTrack()
{
  std::cout << "Window::addTrack()" << std::endl;
  
  guiState.addTrack();
  
  std::stringstream trackName;
  trackName << numTracks;
  
  // label
  tracklabelList.push_back( new Gtk::Label( trackName.str() ) );
  std::list<Gtk::Label*>::iterator labelIter = tracklabelList.begin();
  std::advance(labelIter,numTracks);
  mainTable->attach( **labelIter, numTracks, numTracks+1, 0, 1);
  
  // input selector
  trackinputList.push_back( new Gtk::ComboBoxText() );
  inputIter = trackinputList.begin();
  std::advance(inputIter,numTracks);
  (**inputIter).append_text("input");
  (**inputIter).set_active(0);
  mainTable->attach( **inputIter, numTracks, numTracks+1, 1, 2);
  
  // clip selector
  clipselectorList.push_back( new ClipSelector( top, &guiState ) );
  clipselectorIter = clipselectorList.begin();
  std::advance(clipselectorIter,numTracks);
  mainTable->attach( **clipselectorIter, numTracks, numTracks+1, 2, 3);
  
  // progress bar
  trackprogressList.push_back( new Gtk::ProgressBar() );
  progressIter = trackprogressList.begin();
  std::advance(progressIter,numTracks);
  (**progressIter).set_fraction(numTracks / 8.f);
  //mainTable->attach( **progressIter, numTracks, numTracks+1, 3, 4);
  
  // fader / pan
  trackoutputList.push_back( new TrackOutput( top, &guiState ) );
  std::list<TrackOutput*>::iterator i = trackoutputList.begin();
  std::advance(i,numTracks);
  mainTable->attach( **i, numTracks, numTracks+1, 4, 5);
  
  mainTable->show_all();
  numTracks++;
}

