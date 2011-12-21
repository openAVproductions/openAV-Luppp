
#include "g_window.hpp"

#include "offlineworker.hpp"

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
  currentEffectsTrack = 0;
  previousEffectsTrack= 0;
  
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
  
  refBuilder->get_widget("mainBox", mainBox);
  mainBox->add( waveview );
  mainBox->show_all();
  
  refBuilder->get_widget("mainTable", mainTable);
  refBuilder->get_widget("trackEffectBox", trackEffectBox);
  
  refBuilder->get_widget("menuAddLowpass", menuAddLowpass);
  menuAddLowpass->signal_activate().connect ( sigc::bind (sigc::mem_fun( *this, &Window::addEffect ), EFFECT_LOWPASS ) );
  
  refBuilder->get_widget("menuAddHighpass", menuAddHighpass);
  menuAddHighpass->signal_activate().connect( sigc::bind (sigc::mem_fun( *this, &Window::addEffect ), EFFECT_HIGHPASS) );
  
  refBuilder->get_widget("menuAddBeatsmasher", menuAddBeatsmasher);
  menuAddBeatsmasher->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_BEATSMASH));
  
  refBuilder->get_widget("menuAddParametric", menuAddParametric);
  menuAddParametric->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_PARAMETRIC_EQ));
  
  refBuilder->get_widget("menuAddReverb", menuAddReverb);
  menuAddReverb->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_REVERB));
  
  refBuilder->get_widget("menuAddTransient", menuAddTransient);
  menuAddTransient->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_TRANSIENT));
  
  refBuilder->get_widget("menuAddCompressor", menuAddCompressor);
  menuAddCompressor->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_COMPRESSOR));
  
  addTrack();
  addTrack();
  //addTrack();
  
  // poll the event Queue
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Window::handleEvent), 50);
  
  // last thing, now we're starting the GUI main loop
  kit->run(*window);
}

void Window::addEffect( EffectType et )
{
  // create a new effect of Type et, and offlineWorker will create an EE
  // to tell the JACK thread to insert it into the path. JACK will then
  // send an EE to tell the GUI where / what Effect to insert
  cout << "Window::addEffect()  type = " << (int) et << endl;
  int ret = top->offlineWorker->createNewEffect( currentEffectsTrack, 1, et );
}

int Window::handleEvent()
{
  //cout << "Window::handleEvent()" << endl;
  
  bool moreEventsWaiting = true;
  
  top->scopeVectorMutex.lock();
  {
    waveview.setSample( top->scopeVector );
  }
  top->scopeVectorMutex.unlock();
  waveview.redraw();
  
  
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
    else if ( e->type == EE_TRACK_DEVICE_ACTIVE ) {
      std::cout << "Gui DEVICE ACTIVE   UID: " << e->ia << " value: " << e->ic << std::endl; 
      
      guiState.effectState.at(e->ia).active = e->ic;
      
      redrawEffectBox(); // should be only redrawing current widget
    }
    else if ( e->type == EE_TRACK_SELECT_DEVICE ) {
      std::cout << "Gui TrackSelect event  t: " << e->ia << " d: " << e->ib << std::endl;
      
      if ( e->ia < guiState.trackoutputState.size() )
      {
        guiState.trackoutputState.at(previousEffectsTrack).selected = false;
        guiState.trackoutputState.at(e->ia).selected = true;
        guiState.trackoutputState.at(e->ia).selectedDevice = e->ib;
        
        cout << "set trackOutputState variables, now itering over widget list" << endl;
        
        // redraw the "unselected" trackOutput
        std::list<TrackOutput*>::iterator i = trackoutputList.begin();
        std::advance(i,currentEffectsTrack);
        (*i)->redraw();
        
        // redraw the "selected" trackOutput
        i = trackoutputList.begin();
        std::advance(i,e->ia);
        (*i)->redraw();
        
        // keep the new currentEffectsTrack & previousEffectsTrack
        previousEffectsTrack = currentEffectsTrack;
        currentEffectsTrack = e->ia;
        
        // only redraw the EffectsBox if we have a different track!
        if ( previousEffectsTrack != currentEffectsTrack )
        {
          redrawEffectBox();
        }
      }
      else
      {
        cout << "EE_TRACK_SELECT_DEVICE out of bounds! " << e->ia << endl;
      }
    }
    else if ( e->type == EE_TRACK_SET_SPEED )
    {
      guiState.bufferAudioSourceState.at(e->ia).speed = e->fa;
      
      std::list<Gtk::ProgressBar*>::iterator progIter = trackprogressList.begin();
      advance(progIter,e->ia);
      (*progIter)->set_fraction(e->fa);
    }
    else if ( e->type == EE_LOOPER_LOAD )
    {
      cout << "LOOPER_LOAD event in GUI" << endl;
      if ( e->ia < guiState.clipSelectorState.size() ) {
        // set GuiStateStore value
        std::list<ClipInfo>::iterator iter = guiState.clipSelectorState.at(e->ia).clipInfo.begin();
        std::advance(iter, e->ib);
        
        (*iter).state = CLIP_STATE_LOADED;
        (*iter).bufferID = e->ic;
        
        // gui widget
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
        
        if ( playing >= 0 ) // check list access here
        {
          std::list<ClipInfo>::iterator iter = guiState.clipSelectorState.at(e->ia).clipInfo.begin();
          std::advance(iter, playing);
          (*iter).state = CLIP_STATE_LOADED;
        }
        
        // playing can = -1 for "nothing playing", so set regardless
        guiState.clipSelectorState.at(e->ia).playing = e->ib;
        
        // update currently playing, and set item to playing state
        if ( e->ib >= 0 ) // range check
        {
          std::list<ClipInfo>::iterator iter = guiState.clipSelectorState.at(e->ia).clipInfo.begin();
          std::advance(iter, e->ib);
          (*iter).state = CLIP_STATE_PLAYING;
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
    else if ( e->type == EE_TRACK_SET_PLUGIN_PARAMETER )
    {
      //std::cout << "PLUGIN PARAM t " << e->ia << " pos " << e->ib << " param " << e->ic << " val " << e->fa << std::endl;
      
      if ( e->ia < guiState.effectState.size() )
      {
        // here we are writing based on track, but we should be writing ID
        cout << "UniqueID: " << e->ia << "  param: " << e->ic << "  value" << e->fa << endl;
        guiState.effectState.at(e->ia).values[e->ic] = e->fa;
        redrawEffectBox();
      }
    }
    else if ( e->type == EE_STATE_NEW_EFFECT )
    {
      int t = e->ia;
      int p = e->ib;
      int et= e->ic;
      //cout << "GUI: New effect! t " << t << ", p " << p << ", EffectType " << et<< endl;
      
      
      if ( t < trackVector.size() ) // check track
      {
        cout << "EE_STATE_NEW_EFFECT, pushing new widget to trackVector.at( "<<t<<" ) to the back of the widgetVector"<<endl;
        
        bool newEffect = true;
        switch ( et )
        {
          case EFFECT_LOWPASS: trackVector.at(t).widgetVector.push_back( new GLowPass(top, &guiState) ); break;
          case EFFECT_HIGHPASS: trackVector.at(t).widgetVector.push_back( new GHighPass(top, &guiState) ); break;
          case EFFECT_COMPRESSOR:trackVector.at(t).widgetVector.push_back( new GCompressor(top, &guiState) ); break;
          case EFFECT_PARAMETRIC_EQ:trackVector.at(t).widgetVector.push_back( new Equalizer( &guiState) ); break;
          default: newEffect = false; break;
        }
        
        // only attempt to add the new effect if we added one to the vector
        if ( newEffect )
        {
          // push new EffectState instance onto the EffectState, its
          // sliced per ID, so widgets have thier uniqueID as link to state
          guiState.effectState.push_back( EffectState(-1) );
          
          // add the new widget to the box
          trackEffectBox->add( *trackVector.at(t).widgetVector.back() );
          trackEffectBox->show_all();
          
          currentEffectsTrack = e->ia;
          redrawEffectBox();
        }
      }
      
      cout << " EE_STATE_NEW_EFFECT handling done, now processing other Events" << endl;
    }
    
    /*
    cout << "Deleting engineEvent in GWindow::handleEvent() NOW: " << flush;
    // we've handled the EngineEvent, but it remains in memory, so we delete it:
    //delete e;
    cout << "\tDONE!" << endl;
    */
  }
  
  
  // we've handled the event, now make sure that Engine still has Event pointers
  // available so it doesn't have to "new" them itself
  // this gets called in the GUI timed callback, so it will print details contsantly
  int numEvents = top->toEngineEmptyEventQueue.writeSpaceEngineEventAvailable();
  
  for (int i = 0; i < numEvents; i++ )
  {
    top->toEngineEmptyEventQueue.push( new EngineEvent() );
  }
  
  return true;
}

void Window::redrawEffectBox()
{
  //cout << "Window::redrawEffectBox() currentEffectTrack: " << currentEffectsTrack 
  //     << "previousEffectsTrack: " << previousEffectsTrack << std::endl;
  
  // hide all elements from the previous track
  for(int i = 0;  i < trackVector.at(previousEffectsTrack).widgetVector.size(); i++) {
    trackEffectBox->remove( *trackVector.at(previousEffectsTrack).widgetVector.at(i) );
  }
  
  //cout << "successfully removed all widgets" << endl;
  
  // add each widget in the current track to the box, and show_all() them
  for(int i = 0;  i < trackVector.at(currentEffectsTrack).widgetVector.size(); i++)
  {
    trackEffectBox->add( *trackVector.at(currentEffectsTrack).widgetVector.at(i) );
  }
  
  //cout << "successfully added all new widgets" << endl;
  
  trackEffectBox->show_all();
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
  
  trackVector.push_back( GTrack() );
  
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

