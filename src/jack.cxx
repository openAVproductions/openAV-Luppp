/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "jack.hxx"

#include <sstream>
#include <cstring>
#include <iostream>

#include "state/state.hxx"

#include "gui.hxx"
#include "logic.hxx"
#include "config.hxx"
#include "looper.hxx"
#include "metronome.hxx"
#include "gridlogic.hxx"
#include "trackoutput.hxx"
#include "timemanager.hxx"
#include "controllerupdater.hxx"
#include "jacksendreturn.hxx"
#include "dsp/dsp_reverb.hxx"
#include "dsp/dsp_dbmeter.hxx"

#include "audiobuffer.hxx"
#include "eventhandler.hxx"
#include "controller/genericmidi.hxx"
#include "controller/guicontroller.hxx"


using namespace std;

int AudioBuffer::privateID = 0;

// static pointers from main
extern Gui* gui;
extern Jack* jack;

void Jack::setup(std::string name)
{
  if ( jack == 0 )
  {
    jack = new Jack( name );
    
    jack->activate();
    return;
  }
  else
  {
    LUPPP_WARN("JACK instance already exists!");
  }
}

void Jack::resetMidiBindingState()
{
  // MIDI binding stuff
  bindingEventRecordEnable = false;
  bindingEventType = EVENT_NULL;
  bindingTrack = -2;
  bindingScene = -1;
  bindingSend  = -1;
  bindingActive= -1;
}

Jack::Jack( std::string name ) :
  client( jack_client_open ( name.c_str(), JackNullOption , 0 , 0 ) ),
  state( new State() ),
  controllerUpdater( new ControllerUpdater() ),
  clientActive(false)
{
  jack = this;
  
  samplerate = jack_get_sample_rate( client );
  
  LUPPP_NOTE("Samplerate %i", samplerate );
  
  // construct Observer classes here, not in the initializer list as the Jack*
  // will be 0x0 until then.
  timeManager = new TimeManager(),
  metronome = new Metronome();
  logic = new Logic();
  gridLogic = new GridLogic();
  
  // CAREFUL: setup the size by default: otherwise malloc() is called on push_back
  midiIO.reserve( CONTROLLERS_PREALLOC );
  
  resetMidiBindingState();
  
  //GenericMIDI* tmp = new GenericMIDI("akai_apc.ctlr");
  //tmp->registerComponents();
  //controllerUpdater->registerController( static_cast<Controller*>(tmp) );
  
  buffers.nframes = jack_get_buffer_size( client );
  buffers.samplerate = jack_get_sample_rate( client );
  
  EventSamplerate e(buffers.samplerate);
  writeToGuiRingbuffer( &e );
  
  uiUpdateCounter  = buffers.samplerate / 30;
  uiUpdateConstant = buffers.samplerate / 30;
  
  masterInput  = jack_port_register( client,
                          "master_in",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterReturnL = jack_port_register( client,
                          "master_return_left",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterReturnR = jack_port_register( client,
                          "master_return_right",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsInput,
                          0 );
  
  masterOutputL = jack_port_register( client,
                          "master_left",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  masterOutputR = jack_port_register( client,
                          "master_right",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  headphonesPort = jack_port_register( client,
                          "headphone_out",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sendOutput    = jack_port_register( client,
                          "send_out",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sidechainKeyOutput= jack_port_register( client,
                          "sidechain_key",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  sidechainSignalOutput= jack_port_register( client,
                          "sidechain_signal",
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput,
                          0 );
  
  /*
  masterMidiInput  = jack_port_register( client,
                          "midi_in",
                          JACK_DEFAULT_MIDI_TYPE,
                          JackPortIsInput,
                          0 );
  */
  

  returnVol = 1.0f;
  
  inputToMixEnable  = false;
  inputToSendEnable = false;
  inputToKeyEnable  = false;
  inputToMixVol     = 0.f;
  inputToSendVol    = 0.f;
  inputToXSideVol   = 0.f;
  
  /// prepare internal buffers
  buffers.audio[Buffers::SEND]           = new float[ buffers.nframes ];
  buffers.audio[Buffers::SIDECHAIN_KEY]  = new float[ buffers.nframes ];
  buffers.audio[Buffers::SIDECHAIN_SIGNAL]=new float[ buffers.nframes ];
  
  buffers.audio[Buffers::MASTER_OUT_L]   = new float[ buffers.nframes ];
  buffers.audio[Buffers::MASTER_OUT_R]   = new float[ buffers.nframes ];
  
  for(int i = 0; i < NTRACKS; i++)
  {
    /** Setup the tracks:
     *  The TrackOutput gets a pointer to the next AudioProcessor to call:
     * This is either a JackSendReturn (providing send and return ports)
     *  or the track's Looper instance.
     * This is an option in luppp.prfs
    **/
    loopers.push_back( new Looper(i) );

    if(gui->enableJackSendReturns)
    {
        tracksendreturns.push_back(new JackSendReturn(i,loopers.back(),client));
        trackOutputs.push_back( new TrackOutput(i, tracksendreturns.back() ) );
    }
    else
        trackOutputs.push_back( new TrackOutput(i, loopers.back() ) );

    buffers.audio[Buffers::TRACK_0 + i] = new float[ buffers.nframes ];
    buffers.audio[Buffers::SEND_TRACK_0+i]=new float[buffers.nframes];
    if(gui->enableJackSendReturns)
        buffers.audio[Buffers::RETURN_TRACK_0+i]=new float[buffers.nframes];
    else
        buffers.audio[Buffers::RETURN_TRACK_0+i]=buffers.audio[Buffers::SEND_TRACK_0+i];
    
    timeManager->registerObserver( loopers.back() );
  }
  
  /// setup DSP instances
  inputVol = 1.0f;
  masterVol = 0.75f;
  masterVolLag =0.75f;
  masterVolDiff =0.0f;
  masterMeter = new DBMeter( buffers.samplerate );
  inputMeter  = new DBMeter( buffers.samplerate );
  
  buffers.transportPosition = 0;
  
  /// setup JACK callbacks
  if ( jack_set_process_callback( client,
                                  static_process,
                                  static_cast<void*>(this)) )
  {
    LUPPP_ERROR("%s","Error setting process callback");
  }
  
  if ( jack_set_timebase_callback(client,
                                  0, //0, 0 == must be master, 1 == conditional
                                  (JackTimebaseCallback)static_timebase,
                                  static_cast<void*>(this)) )
  {
    LUPPP_ERROR("%s","Error setting timebase callback");
  }
  
  //Controller* m = new AkaiAPC();
  
  // TODO: Add GUI dialog to add controllers, and insert them into the controller map.
  // Watch out for RT stuff, loading file, registering ports etc: before activate?!
  //Controller* m = new GenericMIDI("akai_apc.ctlr","apc");
  
  Controller* g = new LupppGUI();
  controllerUpdater->registerController( g );
  
  if ( !g )
  {
    LUPPP_ERROR("%s","Error creating LupppGUI Controller instance");
  }
  
  // call into the GUI, telling it to register default controllers
  gui->setupMidiControllers();
  
}

Jack::~Jack()
{
  delete timeManager;
  delete metronome;
  delete state;
  delete logic;
  delete gridLogic;
  delete controllerUpdater;
  
  delete inputMeter;
  delete masterMeter;
    for(int i = 0; i < NTRACKS; i++)
    {
        delete [] buffers.audio[Buffers::TRACK_0+i];
        delete [] buffers.audio[Buffers::SEND_TRACK_0+i];
        if(gui->enableJackSendReturns)
        {
            delete [] buffers.audio[Buffers::RETURN_TRACK_0+i];
            delete tracksendreturns[i];
        }
        delete loopers[i];
        delete trackOutputs[i];
    }
}

void Jack::activate()
{
  jack_activate( client );
}

void Jack::transportRolling(bool rolling)
{
  if(rolling)
    jack_transport_start(client);
  else
    jack_transport_stop(client);
}

void Jack::quit()
{
  //jack_deactivate( client );
  jack_client_close( client );
  LUPPP_NOTE("%s","Quit JACK.");
}

TrackOutput* Jack::getTrackOutput(int t)
{
  if ( t >= 0 && t < NTRACKS )
    return trackOutputs.at(t);
#ifdef DEBUG_TRACKS
  else
  {
    printf( "Jack::getTrackOutput() returning 0x0: invalid track requested!\n" );
  }
#endif
  
  return 0;
}

JackSendReturn* Jack::getJackSendReturn(int t)
{
  if ( t >= 0 && t < NTRACKS )
    return tracksendreturns.at(t);
#ifdef DEBUG_TRACKS
  else
  {
    printf( "Jack::getTrackOutput() returning 0x0: invalid track requested!\n" );
  }
#endif

  return 0;
}


Looper* Jack::getLooper(int t)
{
  if ( t >= 0 && t < NTRACKS )
    return loopers.at(t);
#ifdef DEBUG_TRACKS
  else
  {
    printf( "Jack::getLooper() returning 0x0: invalid track requested!\n" );
  }
#endif
  
  return 0;
}


void Jack::registerMidiIO( MidiIO* mo )
{
  //LUPPP_NOTE("Jack::registerMidiIO()" );
  
  // CAREFUL : this could need to resize and cause malloc() in RT thread
  midiIO.push_back( mo );
}

void Jack::unregisterMidiIO( MidiIO* mo )
{
  LUPPP_NOTE("Jack::unregisterMidiIO()");
  
  // unregister the observer
  for(unsigned int i = 0; i < midiIO.size(); i++)
  {
    if ( midiIO.at(i) == mo )
    {
      cout << "removing mo at " << i << endl;
      midiIO.erase( midiIO.begin() + i );
      return;
    }
  }
}

int Jack::process (jack_nframes_t nframes)
{
  /// get buffers
  buffers.audio[Buffers::MASTER_INPUT]        = (float*)jack_port_get_buffer( masterInput    , nframes );
  buffers.audio[Buffers::MASTER_RETURN_L]     = (float*)jack_port_get_buffer( masterReturnL  , nframes );
  buffers.audio[Buffers::MASTER_RETURN_R]     = (float*)jack_port_get_buffer( masterReturnR  , nframes );
  buffers.audio[Buffers::HEADPHONES_OUT]      = (float*)jack_port_get_buffer( headphonesPort , nframes );
  
  buffers.audio[Buffers::JACK_SEND_OUT]       = (float*)jack_port_get_buffer( sendOutput     , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_L]   = (float*)jack_port_get_buffer( masterOutputL  , nframes );
  buffers.audio[Buffers::JACK_MASTER_OUT_R]   = (float*)jack_port_get_buffer( masterOutputR  , nframes );
  buffers.audio[Buffers::JACK_SIDECHAIN_KEY]   = (float*)jack_port_get_buffer(sidechainKeyOutput,nframes);
  buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL]=(float*)jack_port_get_buffer(sidechainSignalOutput,nframes);
  
  // clear the buffers
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_L] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::JACK_MASTER_OUT_R] , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_L]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::MASTER_OUT_R]      , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::HEADPHONES_OUT]    , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SEND]              , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN_KEY]     , 0, sizeof(float) * nframes );
  memset( buffers.audio[Buffers::SIDECHAIN_SIGNAL]  , 0, sizeof(float) * nframes );
  
  
  //buffers.midi [Buffers::MASTER_MIDI_INPUT]   = (void*) jack_port_get_buffer( masterMidiInput, nframes );
  
  /// init buffers for each MidiIO
  for(unsigned int i = 0; i < midiIO.size(); i++ )
  {
    midiIO.at(i)->initBuffers( nframes );
  }
  
  /// do events from the ringbuffer
  handleDspEvents();
  
  /// process incoming MIDI
  /*
  jack_midi_event_t in_event;
  int masterMidiInputIndex = 0;
  int event_count = (int) jack_midi_get_event_count( buffers.midi[Buffers::MASTER_MIDI_INPUT] );
  while ( masterMidiInputIndex < event_count )
  {
    jack_midi_event_get(&in_event, buffers.midi[Buffers::MASTER_MIDI_INPUT], masterMidiInputIndex);
    
    char buffer [50];
    sprintf (buffer, "MIDI %i %i %i", int(in_event.buffer[0]), int(in_event.buffer[1]), int(in_event.buffer[2]) );
    EventGuiPrint e( buffer );
    writeToGuiRingbuffer( &e );
    
    masterMidiInputIndex++;
  }
  */
  
  
  /// update "time" from JACK master, or write master?
  buffers.transportFrame = jack_get_current_transport_frame(client);
  
  // time manager deals with detecting bar() / beat() events, and calls
  // processFrames() with the appropriate nframes
  timeManager->process( &buffers );
  
  return 0;
}


void Jack::processFrames(int nframes)
{
  if ( nframes < 0 )
  {
    LUPPP_WARN("Jack processFrames got nframes < 0");
    return;
  }
  
  /*
  // extreme debugging of timemanager process-split functionality
  char buffer [50];
  sprintf (buffer, "Jack::processFrames() got %i", nframes );
  EventGuiPrint e2( buffer );
  writeToGuiRingbuffer( &e2 );
  */
  
  /// process each MidiIO registered MIDI port
  for(unsigned int i = 0; i < midiIO.size(); i++ )
  {
    midiIO.at(i)->process( nframes );
  }
  
  /// process each track, starting at output and working up signal path
  for(unsigned int i = 0; i < NTRACKS; i++)
  {
    trackOutputs.at(i)->process( nframes, &buffers );
  }
  
  /// metro signal
  metronome->process( nframes, &buffers );
  
  /// mix input, reverb & post-sidechain in
  for(unsigned int i = 0; i < buffers.nframes; i++)
  {
    float input= buffers.audio[Buffers::MASTER_INPUT][i] * inputVol;
    
    float L    = buffers.audio[Buffers::MASTER_OUT_L][i];
    float R    = buffers.audio[Buffers::MASTER_OUT_R][i];
    float returnL = buffers.audio[Buffers::MASTER_RETURN_L][i];
    float returnR = buffers.audio[Buffers::MASTER_RETURN_R][i];
    
    
    if ( inputToMixEnable )
    {
      // if sending to mix, scale by volume *and* by XSide send
      float tmp = input * inputToMixVol * (1-inputToXSideVol);
      L += tmp;
      R += tmp;
    }
    if ( inputToSendEnable )
    {
      // post-mix-send amount: hence * inputToMixVol
      buffers.audio[Buffers::SEND][i] += input * inputToSendVol * inputToMixVol;
    }
    if ( inputToKeyEnable )
    {
      buffers.audio[Buffers::SIDECHAIN_KEY][i] += input;
    }
    
    
    buffers.audio[Buffers::SIDECHAIN_SIGNAL][i] += input * inputToXSideVol;
    
    //compute master volume lag;
    if(fabs(masterVol-masterVolLag)>=fabs(masterVolDiff/10.0))
        masterVolLag+=masterVolDiff/10.0;
    /// mixdown returns into master buffers
    // FIXME: Returns broken, due to metronome glitch in master output: buffer
    // writing issue or such. See #95 on github
    buffers.audio[Buffers::JACK_MASTER_OUT_L][i] = L * masterVolLag;// (L + returnL*returnVol) * masterVol;
    buffers.audio[Buffers::JACK_MASTER_OUT_R][i] = R * masterVolLag;// (R + returnR*returnVol) * masterVol;
    
    /// write SEND content to JACK port
    buffers.audio[Buffers::JACK_SEND_OUT][i] = buffers.audio[Buffers::SEND][i];
    buffers.audio[Buffers::JACK_SIDECHAIN_KEY][i] = buffers.audio[Buffers::SIDECHAIN_KEY][i];
    buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL][i] = buffers.audio[Buffers::SIDECHAIN_SIGNAL][i];
  }
  
  
  /// db meter on master input & output
  inputMeter->process( nframes, buffers.audio[Buffers::MASTER_INPUT], buffers.audio[Buffers::MASTER_INPUT]);
  masterMeter->process(nframes, buffers.audio[Buffers::JACK_MASTER_OUT_L], buffers.audio[Buffers::JACK_MASTER_OUT_R] );
  
  if ( uiUpdateCounter > uiUpdateConstant )
  {
    // instead of scaling whole buffer, just scale output by vol
    EventTrackSignalLevel e(-1, masterMeter->getLeftDB(), masterMeter->getRightDB() );
    writeToGuiRingbuffer( &e );
    EventTrackSignalLevel e2(-2, inputMeter->getLeftDB() * inputVol, inputMeter->getRightDB() * inputVol );
    writeToGuiRingbuffer( &e2 );
    
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  /*
  // memcpy the internal MASTER_OUTPUT buffer to the JACK_MASTER_OUTPUT
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_L],
          buffers.audio[Buffers::MASTER_OUT_L],
          sizeof(float)*nframes);
  
  memcpy( buffers.audio[Buffers::JACK_MASTER_OUT_R],
          buffers.audio[Buffers::MASTER_OUT_R],
          //buffers.audio[Buffers::POST_SIDECHAIN],
          //buffers.audio[Buffers::SEND],  // uncomment to listen to reverb send only
          sizeof(float)*nframes);
  */
  
  // move buffer pointers up nframes: allows processing of one "nframes" from 
  // JACK in multiple parts internally in Luppp: used for processing bar() / beat()
  // if a full JACK nframes has been processed, this is extra work: its not that expensive
  /// update buffers by nframes
  if(nframes<buffers.nframes)
  {
      buffers.audio[Buffers::MASTER_INPUT]        = &buffers.audio[Buffers::MASTER_INPUT]   [nframes];
      buffers.audio[Buffers::MASTER_RETURN_L]     = &buffers.audio[Buffers::MASTER_RETURN_L][nframes];
      buffers.audio[Buffers::MASTER_RETURN_R]     = &buffers.audio[Buffers::MASTER_RETURN_R][nframes];
      buffers.audio[Buffers::HEADPHONES_OUT]      = &buffers.audio[Buffers::HEADPHONES_OUT] [nframes];

      buffers.audio[Buffers::JACK_SEND_OUT]       = &buffers.audio[Buffers::JACK_SEND_OUT][nframes];
      buffers.audio[Buffers::JACK_MASTER_OUT_L]   = &buffers.audio[Buffers::JACK_MASTER_OUT_L][nframes];
      buffers.audio[Buffers::JACK_MASTER_OUT_R]   = &buffers.audio[Buffers::JACK_MASTER_OUT_R][nframes];
      buffers.audio[Buffers::JACK_SIDECHAIN_KEY]  = &buffers.audio[Buffers::JACK_SIDECHAIN_KEY][nframes];
      buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL]=&buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL][nframes];
  }
  
  return;
}

void Jack::clearInternalBuffers(int nframes)
{
    memset(buffers.audio[Buffers::SEND],0,sizeof(float)*nframes);
    memset(buffers.audio[Buffers::SIDECHAIN_KEY],0,sizeof(float)*nframes);
    memset(buffers.audio[Buffers::SIDECHAIN_SIGNAL],0,sizeof(float)*nframes);
    memset(buffers.audio[Buffers::MASTER_OUT_L],0,sizeof(float)*nframes);
    memset(buffers.audio[Buffers::MASTER_OUT_R],0,sizeof(float)*nframes);
    for(int i=0;i<NTRACKS;i++)
    {
        memset(buffers.audio[Buffers::TRACK_0 + i],0,sizeof(float)*nframes);
        memset(buffers.audio[Buffers::SEND_TRACK_0 + i],0,sizeof(float)*nframes);
        if(gui->enableJackSendReturns)
            memset(buffers.audio[Buffers::RETURN_TRACK_0 + i],0,sizeof(float)*nframes);
    }
}

void Jack::masterVolume(float vol)
{
  masterVol = vol;
  masterVolDiff=masterVol-masterVolLag;
}

void Jack::returnVolume(float vol)
{
  returnVol = vol;
}

void Jack::inputVolume(float v)
{
  inputVol = v * 2;
}

void Jack::inputTo(INPUT_TO to, float v)
{
  switch ( to )
  {
    case INPUT_TO_MIX:
        inputToMixVol = v;
        break;
    case INPUT_TO_SEND:
        inputToSendVol = v;
        break;
    case INPUT_TO_XSIDE:
        inputToXSideVol = v;
        break;
    default:
        break;
  }
}

void Jack::inputToActive(INPUT_TO to, bool a)
{
  switch ( to )
  {
    case INPUT_TO_MIX:
        inputToMixEnable = a;
        break;
    case INPUT_TO_SEND:
        inputToSendEnable = a;
        break;
    case INPUT_TO_SIDE_KEY:
        inputToKeyEnable = a;
        break;
    default:
        break;
  }
}

int Jack::getBuffersize()
{
  return jack_get_buffer_size( client );
}

int Jack::getSamplerate()
{
  return samplerate;
}

int Jack::timebase(jack_transport_state_t state,
                   jack_nframes_t nframes,
                   jack_position_t* pos,
                   int newPos)
{
  // fill buffers with data, then pass to timeManager
  buffers.transportPosition = pos;
  buffers.transportState    =&state;
  
  return 0;
}

int Jack::static_process(jack_nframes_t nframes, void *instance)
{
  return static_cast<Jack*>(instance)->process(nframes);
}

int Jack::static_timebase(jack_transport_state_t state,
                          jack_nframes_t nframes,
                          jack_position_t* pos,
                          int newPos,
                          void* instance)
{
  return static_cast<Jack*>(instance)->timebase(state,nframes, pos, newPos );
}
