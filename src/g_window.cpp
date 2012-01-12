
#include "g_window.hpp"

#include "offlineworker.hpp"
#include "g_buffersource.hpp"

using namespace std;

#include "top.hpp"
#include <sstream>

#include "g_reverb.hpp"
#include "g_lowpass.hpp"
#include "g_limiter.hpp"
#include "g_highpass.hpp"
#include "g_equalizer.hpp"
#include "g_beatsmash.hpp"
#include "g_transient.hpp"
#include "g_compressor.hpp"
#include "g_lvtwodisplay.hpp"

Window::Window(Gtk::Main *k, Top* t) :
  masterOutput(t, &guiState),
  fileSelector(t, &guiState),
  effectSelector(t, &guiState),
  instrumentSelector(t, &guiState)
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
  
  refBuilder->get_widget("fileChooserPane", fileChooserPane);
  fileChooserPane->set_position(0);
  
  // left pane selection widgets
  refBuilder->get_widget("fileChooserViewport", fileChooserViewport);
  fileChooserViewport->add( fileSelector );
  fileSelector.show();
  
  refBuilder->get_widget("effectChooserBox", effectChooserBox);
  effectChooserBox->pack_start( effectSelector, true, true );
  effectSelector.show();
  
  refBuilder->get_widget("instrumentChooserBox", instrumentChooserBox);
  instrumentChooserBox->pack_start( instrumentSelector, true, true );
  instrumentSelector.show();
  
  refBuilder->get_widget("fileBrowserToggle", fileBrowserToggle);
  fileBrowserToggle->signal_clicked().connect ( sigc::mem_fun( *this, &Window::setFileChooserPane ) );
  
  refBuilder->get_widget("mainTable", mainTable);
  
  refBuilder->get_widget("masterOutputBox", masterOutputBox);
  
  // master track widgets
  Gtk::VBox* tmpVbox = new Gtk::VBox();
  
  Gtk::EventBox* tmpBox = new Gtk::EventBox();
  Gtk::Label* tmpLabel = new Gtk::Label( "Master" );
  tmpBox->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
  tmpBox->add( *tmpLabel );
  
  ClipSelector* masterClipSelector = new ClipSelector(top, &guiState, true);
  tmpVbox->pack_end( *masterClipSelector, true, true );
  
  tmpVbox->add( *tmpBox );
  masterOutputBox->add( *tmpVbox );
  
  // grab widgets & connect signal handlers for DnD
  refBuilder->get_widget("trackEffectBox", trackEffectBox);
  refBuilder->get_widget("trackEffectEventBox", trackEffectEventBox);
  
  // drop destination
  std::vector<Gtk::TargetEntry> listTargets;
  listTargets.push_back( Gtk::TargetEntry("INSTRUMENT_STRING") );
  listTargets.push_back( Gtk::TargetEntry( "EFFECT_STRING" ) );
  trackEffectEventBox->drag_dest_set(listTargets);
  
  trackEffectEventBox->signal_drag_data_received().connect(sigc::mem_fun(*this, &Window::trackEffectDragDrop) );
  
  //trackEffectEventBox->set_events(Gdk::BUTTON_PRESS_MASK);
  //trackEffectEventBox->signal_button_press_event().connect( sigc::mem_fun(*this, &Window::on_eventbox_button_press) );
  
  //masterOutputBox->add( inputWaveview );
  //masterOutputBox->add( waveview );
  
  masterOutputBox->pack_end( masterOutput, false, true, 0 );
  masterOutputBox->show_all();
  
  refBuilder->get_widget("eeLabel", eeLabel);
  
  refBuilder->get_widget("menuFileAddTrack", menuFileAddTrack);
  menuFileAddTrack->signal_activate().connect ( sigc::mem_fun( *this, &Window::sendAddTrack ) );
  
  refBuilder->get_widget("menuFileQuit", menuFileQuit);
  menuFileQuit->signal_activate().connect ( sigc::mem_fun( *this, &Window::quit ) );
  
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
  
  refBuilder->get_widget("menuAddLimiter", menuAddLimiter);
  menuAddLimiter->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_LIMITER));
  
  refBuilder->get_widget("menuAddTrancegate", menuAddTrancegate);
  menuAddTrancegate->signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &Window::addEffect ), EFFECT_TRANCEGATE));
  
  // poll the event Queue
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Window::handleEvent), 50);
  
  // create a track when first idle
  Glib::signal_idle().connect( sigc::bind_return( sigc::mem_fun(*this, &Window::sendAddTrack ), false ) );
  
  window->set_default_icon_from_file( "lupppIcon.png" );
  window->set_default_size( 1000, 350 );
  
  // last thing, now we're starting the GUI main loop
  kit->run(*window);
}

void Window::setFileChooserPane()
{
  if ( fileBrowserToggle->get_active() )
  {
    //cout << "FileBrowserToggle active, px = 200" << endl;
    fileChooserPane->set_position( 200 );
  }
  else
  {
    //cout << "FileBrowserToggle disabled, px = 0" << endl;
    fileChooserPane->set_position( 0 );
  }
}

void Window::trackEffectDragDrop(const Glib::RefPtr<Gdk::DragContext>& context, int, int,
                                 const Gtk::SelectionData& selection_data, guint, guint time)
{
  cout << "Window::trackEffectDragDrop() called!" << endl;
  
  const int length = selection_data.get_length();
  if( (length >= 0) )
  {
    std::string filename = selection_data.get_data_as_string();
    std::cout << "Received " << filename << " in trackEffectDragDrop! " << std::endl;
    int ret = top->offlineWorker->createNewEffect( currentEffectsTrack, 1, EFFECT_TESTTONES );
    
    
  }
  
}

void Window::quit()
{
  cout << "menuFileQuit clicked, exiting now!" << endl;
  kit->quit();
}

void Window::sendAddTrack()
{
  //cout << "GUI got AddTrack menu activation, sending ADD_TRACK event to Engine" << endl;
  EngineEvent* x = new EngineEvent();
  // -1 has no meaning here
  x->addTrack(-1);
  top->toEngineQueue.push(x);
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
    inputWaveview.setSample( top->inputScopeVector );
    waveview.setSample( top->scopeVector );
  }
  top->scopeVectorMutex.unlock();
  waveview.redraw();
  inputWaveview.redraw();
  
  
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
    else if ( e->type == EE_ADD_TRACK ) {
      //cout << "GUI got ADD_TRACK event, sending to OfflineWorker  NewID = " << e->ia << endl;
      int ret = top->offlineWorker->addTrack(e->ia);
      
      if ( ret )
      {
        addTrack();
      }
      
    }
    else if ( e->type == EE_TRACK_RMS ) {
      //cout << "Track " << e->ia << "  RMS: " << e->fa << endl;
      guiState.trackoutputState.at(e->ia).rms = e->fa;
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
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
      guiState.trackoutputState.at(e->ia).recEnable = e->ib;
      std::list<TrackOutput*>::iterator i = trackoutputList.begin();
      std::advance(i,e->ia);
      (*i)->redraw();
    }
    else if ( e->type == EE_LOOPER_PROGRESS ) {
      //cout << "Gui LOOPER_PROGRESS UID = " << e->ia  << " value = " << e->fa << endl;
      guiState.bufferAudioSourceState.at(e->ia).index = e->fa;
      
      if ( e->ia < progressWidgetVector.size() )
      {
        progressWidgetVector.at(e->ia)->setValue( e->fa );
        redrawEffectBox();
      }
    }
    else if ( e->type == EE_LOOPER_RECORD ) {
      cout << "GUI: Looper Record event! t = " << e->ia << "  block: " << e->ib << "  rec? = " << e->ic << endl;
      // we get a "record off" event, and then read *all* the contents of
      // the ringbuffer into a AudioBuffer, load that into engine same as
      // a file source, and finally set the BufferAudioSource to play back
      // that bufferID trough the bufferAudioSourceState list in *engine* statestore
      if ( e->ic == false )
      {
        // create new buffer, get pointer, read space, resize buffer
        AudioBuffer* buffer = new AudioBuffer();
        vector<float>* pntr = buffer->getPointer();
        int readSpace = top->recordAudioQueue.readSpaceAvailable();
        pntr->resize(readSpace);
        
        //cout << "AudioBuffer size before read: " << pntr->size() << flush;
        
        if ( pntr->size() != 0 ) // if two tracks are recording at the same time, the 2nd will have a size 0
        {
          // read from ringbuffer *directly* into AudioBuffer
          top->recordAudioQueue.writeSamplesTo( &pntr->at(0) );
          
          //cout << "   and after " << pntr->size() << endl;
          
          // send new AudioBuffer event to engine State
          EngineEvent* x = new EngineEvent();
          x->setStateAudioBuffer( (void*) buffer);
          top->toEngineQueue.push(x);
          
          cout << "\t\t\tGWindow: LOOPER_RECORD: Sending buffer to " << e->ia << "  " << e->ib << "  " << buffer->getID() << endl;
          // send LooperLoad event
          x = new EngineEvent();
          x->looperLoad( e->ia, e->ib, buffer->getID() );
          top->toEngineQueue.push(x);
          
          // set recording block to -1, as we're not recording 
          guiState.clipSelectorState.at(e->ia).recording = -1; // no recording block
          //cout << "read samples available " << readSpace << endl;
        }
        else
        {
          cout << "GWindow LOOPER_RECORD: Error, size in record == 0, so something went wrong.\nMost likely you pressed tried to record on two tracks at once. This is not supported yet!" << endl;
        }
        
      }
      else // record "on" message
      {
        std::cout << "record on message in GUI thread" << std::endl;
        guiState.trackoutputState.at(e->ia).recEnable = true;
        guiState.clipSelectorState.at(e->ia).recording = e->ib; // inform the clip selector which block
        std::cout << " Setting State.recording on track " << e->ia << "  to value " << guiState.clipSelectorState.at(e->ia).recording << endl;
        
        std::list<ClipSelector*>::iterator clipIter = clipselectorList.begin();
        std::advance(clipIter,e->ia);
        (*clipIter)->redraw();
      }
      
      // update "REC" button in GUI
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
      std::cout << "Gui DEVICE ACTIVE   UID: " << e->ia << " value: " << e->ib << std::endl; 
      
      guiState.effectState.at(e->ia).active = e->ib;
      
      redrawEffectBox(); // should be only redrawing current widget
    }
    else if ( e->type == EE_TRACK_SELECT_DEVICE ) {
      //std::cout << "Gui TrackSelect event  t: " << e->ia << " d: " << e->ib << std::endl;
      
      if ( e->ia < guiState.trackoutputState.size() )
      {
        // iter over all other tracks, and unset "selected"
        for ( int i = 0; i < guiState.trackoutputState.size(); i++)
          guiState.trackoutputState.at(i).selected = false;
        
        guiState.trackoutputState.at(e->ia).selected = true;
        guiState.trackoutputState.at(e->ia).selectedDevice = e->ib;
        
        //cout << "set trackOutputState variables, now itering over widget list" << endl;
        
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
      // set BufferAudioSourceState speed value
      guiState.bufferAudioSourceState.at(e->ia).speed = e->fa;
    }
    else if ( e->type == EE_LOOPER_LOAD )
    {
      cout << "LOOPER_LOAD event in GUI" << endl;
      if ( e->ia < guiState.clipSelectorState.size() ) {
        // now we update the GUI statestore so that we show the slot has been loaded
        std::list<ClipInfo>::iterator iter = guiState.clipSelectorState.at(e->ia).clipInfo.begin();    
        std::advance(iter, e->ib);
        iter->hasBuffer = true;
        
        //(*iter).bufferID = e->ic;?
        
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
        }
        
        // playing can = -1 for "nothing playing", so set regardless
        guiState.clipSelectorState.at(e->ia).playing = e->ib;
        list<ClipInfo>::iterator iter = guiState.clipSelectorState.at(e->ia).clipInfo.begin();
        advance(iter, e->ib);
        if ( !iter->hasBuffer )
        {
          // if the clip doesn't have a buffer, set "stop" clip to play
          guiState.clipSelectorState.at(e->ia).playing = -1;
        }
        
        if ( guiState.clipSelectorState.at(e->ia).playing == -1 )
          tracklabelBoxList.back()->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
        else
          tracklabelBoxList.back()->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("FF6800"));
        
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
          case EFFECT_REVERB:       trackVector.at(t).widgetVector.push_back( new GReverb     (top, &guiState) ); break;
          case EFFECT_LOWPASS:      trackVector.at(t).widgetVector.push_back( new GLowPass    (top, &guiState) ); break;
          case EFFECT_HIGHPASS:     trackVector.at(t).widgetVector.push_back( new GHighPass   (top, &guiState) ); break;
          case EFFECT_BEATSMASH:    trackVector.at(t).widgetVector.push_back( new GBeatSmash  (top, &guiState) ); break;
          case EFFECT_TRANCEGATE:   trackVector.at(t).widgetVector.push_back( new GBeatSmash  (top, &guiState) ); break;
          case EFFECT_TRANSIENT:    trackVector.at(t).widgetVector.push_back( new GTransient  (top, &guiState) ); break;
          case EFFECT_COMPRESSOR:   trackVector.at(t).widgetVector.push_back( new GCompressor (top, &guiState) ); break;
          case EFFECT_LIMITER:      trackVector.at(t).widgetVector.push_back( new GLimiter    (top, &guiState) ); break;
          case EFFECT_PARAMETRIC_EQ:trackVector.at(t).widgetVector.push_back( new Equalizer   (     &guiState) ); break;
          case EFFECT_TESTTONES:    trackVector.at(t).widgetVector.push_back( new Lv2Display  (top, &guiState) ); cout << "creating Lv2Display now" << endl; break;
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
  
  // convert int to string
  std::stringstream s;
  int tmp = (int)(((2000 - numEvents) / 2000.f)* 100);
  if ( tmp > 99 ) tmp = 99;
  s << tmp << "%";
  eeLabel->set_label( s.str() );
  
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
    trackEffectBox->remove( *trackVector.at(currentEffectsTrack).widgetVector.at(i) );
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
  //std::cout << "Window::addTrack()" << std::endl;
  
  guiState.addTrack();
  
  trackVector.push_back( GTrack() );
  trackVector.back().widgetVector.push_back( new GBufferSource(top, &guiState) );
  redrawEffectBox();
  
  std::stringstream trackName;
  trackName << numTracks;
  
  // label
  tracklabelList.push_back( new Gtk::Label( trackName.str() ) );
  std::list<Gtk::Label*>::iterator labelIter = tracklabelList.begin();
  std::advance(labelIter,numTracks);
  
  tracklabelBoxList.push_back( new Gtk::EventBox() );
  tracklabelBoxList.back()->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
  
  tracklabelBoxList.back()->add(**labelIter);
  
  mainTable->attach( *tracklabelBoxList.back(), numTracks, numTracks+1, 0, 1);
  
  // input selector
  trackinputList.push_back( new Gtk::ComboBoxText() );
  inputIter = trackinputList.begin();
  std::advance(inputIter,numTracks);
  (**inputIter).append_text("input");
  (**inputIter).set_active(0);
  //mainTable->attach( **inputIter, numTracks, numTracks+1, 1, 2);
  
  // clip selector
  clipselectorList.push_back( new ClipSelector( top, &guiState ) );
  clipselectorIter = clipselectorList.begin();
  std::advance(clipselectorIter,numTracks);
  mainTable->attach( **clipselectorIter, numTracks, numTracks+1, 2, 3);
  
  // progress bar
  progressWidgetVector.push_back( new GProgress() );
  progressWidgetVector.back()->setValue( 0.f );
  Gtk::Box* tmpVbox = new Gtk::HBox();
  tmpVbox->add( *progressWidgetVector.back() );
  mainTable->attach( *tmpVbox, numTracks, numTracks+1, 3, 4);
  
  // fader / pan
  trackoutputList.push_back( new TrackOutput( top, &guiState ) );
  std::list<TrackOutput*>::iterator i = trackoutputList.begin();
  std::advance(i,numTracks);
  mainTable->attach( **i, numTracks, numTracks+1, 4, 5);
  
  mainTable->show_all();
  numTracks++;
}

