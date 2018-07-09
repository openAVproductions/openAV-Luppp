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
	if ( jack == 0 ) {
		jack = new Jack( name );

		jack->activate();
		return;
	} else {
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
	bindingActive= 1;
}

Jack::Jack( std::string name ) :
	client( jack_client_open ( name.c_str(), JackNullOption, 0, 0 ) ),
	state( new State() ),
	controllerUpdater( new ControllerUpdater() ),
	clientActive(false)
{
	jack = this;
	lastnframes=0;
	samplerate = jack_get_sample_rate( client );

	// construct Observer classes here, not in the initializer list as the Jack*
	// will be 0x0 until then.
	timeManager = new TimeManager(),
	metronome = new Metronome();
	logic = new Logic();
	gridLogic = new GridLogic();

	// CAREFUL: setup the size by default: otherwise malloc() is called on push_back
	midiIO.reserve( CONTROLLERS_PREALLOC );

	resetMidiBindingState();

	buffers.nframes = jack_get_buffer_size( client );
	buffers.samplerate = jack_get_sample_rate( client );

	EventSamplerate e(buffers.samplerate);
	writeToGuiRingbuffer( &e );

	uiUpdateCounter  = buffers.samplerate / 30;
	uiUpdateConstant = buffers.samplerate / 30;

	masterInputL  = jack_port_register( client,
	                                    "master_in_left",
	                                    JACK_DEFAULT_AUDIO_TYPE,
	                                    JackPortIsInput,
	                                    0 );

	masterInputR  = jack_port_register( client,
	                                    "master_in_right",
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

	headphonesPortL = jack_port_register( client,
	                                      "headphone_out_l",
	                                      JACK_DEFAULT_AUDIO_TYPE,
	                                      JackPortIsOutput,
	                                      0 );

	headphonesPortR = jack_port_register( client,
	                                      "headphone_out_r",
	                                      JACK_DEFAULT_AUDIO_TYPE,
	                                      JackPortIsOutput,
	                                      0 );

	sendOutputL   = jack_port_register( client,
	                                    "send_out_l",
	                                    JACK_DEFAULT_AUDIO_TYPE,
	                                    JackPortIsOutput,
	                                    0 );

	sendOutputR   = jack_port_register( client,
	                                    "send_out_r",
	                                    JACK_DEFAULT_AUDIO_TYPE,
	                                    JackPortIsOutput,
	                                    0 );

	sidechainKeyOutputL = jack_port_register( client,
	                      "sidechain_key_l",
	                      JACK_DEFAULT_AUDIO_TYPE,
	                      JackPortIsOutput,
	                      0 );

	sidechainKeyOutputR = jack_port_register( client,
	                      "sidechain_key_r",
	                      JACK_DEFAULT_AUDIO_TYPE,
	                      JackPortIsOutput,
	                      0 );

	sidechainSignalOutputL = jack_port_register( client,
	                         "sidechain_signal_l",
	                         JACK_DEFAULT_AUDIO_TYPE,
	                         JackPortIsOutput,
	                         0 );

	sidechainSignalOutputR = jack_port_register( client,
	                         "sidechain_signal_r",
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
	returnVolLag = 1.0f;
	inputToMixEnable  = false;
	inputToMixEnableLag = 0.f;
	inputToSendEnable = false;
	inputToKeyEnable  = false;
	inputToKeyEnableLag = 0.f;
	inputToMixVol     = 0.f;
	inputToMixVolLag  = 0.f;
	inputToSendVol    = 0.f;
	inputToSendVolLag = 0.f;
	inputToXSideVol   = 0.f;
	inputToXSideVolLag = 0.f;

	/// prepare internal buffers
	buffers.audio[Buffers::SEND_L]            = new float[ buffers.nframes ];
	buffers.audio[Buffers::SEND_R]            = new float[ buffers.nframes ];
	buffers.audio[Buffers::SIDECHAIN_KEY_L]   = new float[ buffers.nframes ];
	buffers.audio[Buffers::SIDECHAIN_KEY_R]   = new float[ buffers.nframes ];
	buffers.audio[Buffers::SIDECHAIN_SIGNAL_L]= new float[ buffers.nframes ];
	buffers.audio[Buffers::SIDECHAIN_SIGNAL_R]= new float[ buffers.nframes ];

	buffers.audio[Buffers::MASTER_OUT_L]   = new float[ buffers.nframes ];
	buffers.audio[Buffers::MASTER_OUT_R]   = new float[ buffers.nframes ];

	// Traverse tracks using the first track as offset
	for(int track = 0; track < NTRACKS; track++) {
		/** Setup the tracks:
		 *  The TrackOutput gets a pointer to the next AudioProcessor to call:
		 * This is either a JackSendReturn (providing send and return ports)
		 *  or the track's Looper instance.
		 * This is an option in luppp.prfs
		**/
		loopers.push_back( new Looper(track) );

		tracksendreturns.push_back(new JackSendReturn(track,loopers.back(),client));
		trackOutputs.push_back( new TrackOutput(track, tracksendreturns.back() ) );

		// index = first-track + (track * channels)
		int trackoffset = track * NCHANNELS;
		buffers.audio[Buffers::TRACK_0_L + trackoffset] = new float[ buffers.nframes ];
		buffers.audio[Buffers::TRACK_0_R + trackoffset] = new float[ buffers.nframes ];
		buffers.audio[Buffers::SEND_TRACK_0_L+trackoffset]=new float[buffers.nframes];
		buffers.audio[Buffers::SEND_TRACK_0_R+trackoffset]=new float[buffers.nframes];
		buffers.audio[Buffers::RETURN_TRACK_0_L+trackoffset]=new float[buffers.nframes];
		buffers.audio[Buffers::RETURN_TRACK_0_R+trackoffset]=new float[buffers.nframes];

		timeManager->registerObserver( loopers.back() );
		if(gui->enablePerTrackOutput) {
			char name[50];
			// Left channel
			sprintf(name,"track_%d_l",track + 1);
			trackJackOutputPorts[trackoffset]=jack_port_register( client,
			                        name,
			                        JACK_DEFAULT_AUDIO_TYPE,
			                        JackPortIsOutput,
			                        0 );
			// Right channel
			sprintf(name,"track_%d_r",track + 1);
			trackJackOutputPorts[trackoffset+1]=jack_port_register( client,
			                          name,
			                          JACK_DEFAULT_AUDIO_TYPE,
			                          JackPortIsOutput,
			                          0 );

		}
	}

	/// setup DSP instances
	inputVol = 1.0f;
	inputVolLag = 1.0f;
	masterVol = 0.75f;
	masterVolLag =0.75f;

	masterMeter = new DBMeter( buffers.samplerate );
	inputMeter  = new DBMeter( buffers.samplerate );

	buffers.transportPosition = 0;

	/// setup JACK callbacks
	if ( jack_set_process_callback( client,
	                                static_process,
	                                static_cast<void*>(this)) ) {
		LUPPP_ERROR("%s","Error setting process callback");
	}

	if ( jack_set_timebase_callback(client,
	                                0, //0, 0 == must be master, 1 == conditional
	                                (JackTimebaseCallback)static_timebase,
	                                static_cast<void*>(this)) ) {
		LUPPP_ERROR("%s","Error setting timebase callback");
	}

	//Controller* m = new AkaiAPC();

	// TODO: Add GUI dialog to add controllers, and insert them into the controller map.
	// Watch out for RT stuff, loading file, registering ports etc: before activate?!
	//Controller* m = new GenericMIDI("akai_apc.ctlr","apc");

	Controller* g = new LupppGUI();
	controllerUpdater->registerController( g );

	if ( !g ) {
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

	// Traverse tracks using the first track as offset
	for(int track = 0; track < NTRACKS; track++) {
		// index = first-track + (track * channels)
		int trackoffset = track * NCHANNELS;
		delete [] buffers.audio[Buffers::TRACK_0_L+trackoffset];
		delete [] buffers.audio[Buffers::TRACK_0_R+trackoffset];
		delete [] buffers.audio[Buffers::SEND_TRACK_0_L+trackoffset];
		delete [] buffers.audio[Buffers::SEND_TRACK_0_R+trackoffset];
		delete [] buffers.audio[Buffers::RETURN_TRACK_0_L+trackoffset];
		delete [] buffers.audio[Buffers::RETURN_TRACK_0_R+trackoffset];
		delete tracksendreturns[track];
		delete loopers[track];
		delete trackOutputs[track];
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
	else {
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
	else {
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
	else {
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
	for(unsigned int i = 0; i < midiIO.size(); i++) {
		if ( midiIO.at(i) == mo ) {
			cout << "removing mo at " << i << endl;
			midiIO.erase( midiIO.begin() + i );
			return;
		}
	}
}

int Jack::process (jack_nframes_t nframes)
{
	/// get buffers
	buffers.audio[Buffers::MASTER_INPUT_L]        = (float*)jack_port_get_buffer( masterInputL,           nframes );
	buffers.audio[Buffers::MASTER_INPUT_R]        = (float*)jack_port_get_buffer( masterInputR,           nframes );
	buffers.audio[Buffers::MASTER_RETURN_L]       = (float*)jack_port_get_buffer( masterReturnL,          nframes );
	buffers.audio[Buffers::MASTER_RETURN_R]       = (float*)jack_port_get_buffer( masterReturnR,          nframes );
	buffers.audio[Buffers::HEADPHONES_OUT_L]      = (float*)jack_port_get_buffer( headphonesPortL,        nframes );
	buffers.audio[Buffers::HEADPHONES_OUT_R]      = (float*)jack_port_get_buffer( headphonesPortR,        nframes );

	buffers.audio[Buffers::JACK_SEND_OUT_L]        = (float*)jack_port_get_buffer( sendOutputL,           nframes );
	buffers.audio[Buffers::JACK_SEND_OUT_R]        = (float*)jack_port_get_buffer( sendOutputR,           nframes );
	buffers.audio[Buffers::JACK_MASTER_OUT_L]      = (float*)jack_port_get_buffer( masterOutputL,         nframes );
	buffers.audio[Buffers::JACK_MASTER_OUT_R]      = (float*)jack_port_get_buffer( masterOutputR,         nframes );
	buffers.audio[Buffers::JACK_SIDECHAIN_KEY_L]   = (float*)jack_port_get_buffer( sidechainKeyOutputL,   nframes );
	buffers.audio[Buffers::JACK_SIDECHAIN_KEY_R]   = (float*)jack_port_get_buffer( sidechainKeyOutputR,   nframes );
	buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_L]= (float*)jack_port_get_buffer( sidechainSignalOutputL,nframes );
	buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_R]= (float*)jack_port_get_buffer( sidechainSignalOutputR,nframes );
	if(gui->enablePerTrackOutput) {
		// Traverse tracks using the first track as offset
		for(int track=0; track<NTRACKS; track++) {
			// index = first-track + (track * channels)
			int trackoffset = track * NCHANNELS;
			buffers.audio[Buffers::JACK_TRACK_0_L+trackoffset] = (float*)jack_port_get_buffer( trackJackOutputPorts[trackoffset],   nframes );
			buffers.audio[Buffers::JACK_TRACK_0_R+trackoffset] = (float*)jack_port_get_buffer( trackJackOutputPorts[trackoffset+1], nframes );
		}
	}


	// clear the buffers
	memset( buffers.audio[Buffers::JACK_MASTER_OUT_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::JACK_MASTER_OUT_R], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::MASTER_OUT_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::MASTER_OUT_R], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::HEADPHONES_OUT_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::HEADPHONES_OUT_R], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SEND_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SEND_R], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SIDECHAIN_KEY_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SIDECHAIN_KEY_R], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SIDECHAIN_SIGNAL_L], 0, sizeof(float) * nframes );
	memset( buffers.audio[Buffers::SIDECHAIN_SIGNAL_R], 0, sizeof(float) * nframes );
	if(gui->enablePerTrackOutput) {
		// Traverse tracks using the first track as offset
		for(int track=0; track<NTRACKS; track++) {
			// index = first-track + (track * channels)
			int trackoffset = track * NCHANNELS;
			memset( buffers.audio[Buffers::JACK_TRACK_0_L+trackoffset], 0, sizeof(float) * nframes );
			memset( buffers.audio[Buffers::JACK_TRACK_0_R+trackoffset], 0, sizeof(float) * nframes );
		}
	}


	//buffers.midi [Buffers::MASTER_MIDI_INPUT]   = (void*) jack_port_get_buffer( masterMidiInput, nframes );

	/// init buffers for each MidiIO
	for(unsigned int i = 0; i < midiIO.size(); i++ ) {
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
	if ( nframes < 0 ) {
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
	for(unsigned int i = 0; i < midiIO.size(); i++ ) {
		midiIO.at(i)->process( nframes );
	}

	/// process each track, starting at output and working up signal path
	for(unsigned int i = 0; i < NTRACKS; i++) {
		trackOutputs.at(i)->process( nframes, &buffers );
	}

	/// metro signal
	metronome->process( nframes, &buffers );

	/// mix input, reverb & post-sidechain in
	for(unsigned int i = 0; i < nframes; i++) {
		// compute *lags für smoothing
		inputToMixVolLag += SMOOTHING_CONST * (inputToMixVol - inputToMixVolLag);
		inputToSendVolLag += SMOOTHING_CONST * (inputToSendVol - inputToSendVolLag);
		inputToXSideVolLag += SMOOTHING_CONST * (inputToXSideVol - inputToXSideVolLag);
		returnVolLag += SMOOTHING_CONST * (returnVol - returnVolLag);
		inputVolLag += SMOOTHING_CONST * (inputVol - inputVolLag);

		inputToKeyEnableLag += SMOOTHING_CONST * (inputToKeyEnable - inputToKeyEnableLag);
		inputToMixEnableLag += SMOOTHING_CONST * (inputToMixEnable - inputToMixEnableLag);

		float inputL = buffers.audio[Buffers::MASTER_INPUT_L][i] * inputVolLag;
		float inputR = buffers.audio[Buffers::MASTER_INPUT_R][i] * inputVolLag;

		float L    = buffers.audio[Buffers::MASTER_OUT_L][i];
		float R    = buffers.audio[Buffers::MASTER_OUT_R][i];
		float returnL = buffers.audio[Buffers::MASTER_RETURN_L][i];
		float returnR = buffers.audio[Buffers::MASTER_RETURN_R][i];

		// if sending to mix, scale by volume *and* by XSide send
		float tmpL = inputL * inputToMixVolLag * (1-inputToXSideVolLag) * inputToMixEnableLag;
		float tmpR = inputR * inputToMixVolLag * (1-inputToXSideVolLag) * inputToMixEnableLag;
		L += tmpL;
		R += tmpR;
		
		if ( inputToSendEnable ) {
			// post-mix-send amount: hence * inputToMixVol
			buffers.audio[Buffers::SEND_L][i] += inputL * inputToSendVolLag * inputToMixVolLag * inputToMixEnableLag;
			buffers.audio[Buffers::SEND_R][i] += inputR * inputToSendVolLag * inputToMixVolLag * inputToMixEnableLag;
		}
		
		
		buffers.audio[Buffers::SIDECHAIN_KEY_L][i] += inputL * inputToKeyEnableLag;
		buffers.audio[Buffers::SIDECHAIN_KEY_R][i] += inputR * inputToKeyEnableLag;
		

		buffers.audio[Buffers::SIDECHAIN_SIGNAL_L][i] += inputL * inputToXSideVolLag;
		buffers.audio[Buffers::SIDECHAIN_SIGNAL_R][i] += inputR * inputToXSideVolLag;

		//compute master volume lag;
		masterVolLag += SMOOTHING_CONST * (masterVol - masterVolLag);
		/// mixdown returns into master buffers
		buffers.audio[Buffers::JACK_MASTER_OUT_L][i] = (L + returnL*returnVolLag) * masterVolLag;
		buffers.audio[Buffers::JACK_MASTER_OUT_R][i] = (R + returnR*returnVolLag) * masterVolLag;

		/// write SEND content to JACK port
		buffers.audio[Buffers::JACK_SEND_OUT_L][i] = buffers.audio[Buffers::SEND_L][i];
		buffers.audio[Buffers::JACK_SEND_OUT_R][i] = buffers.audio[Buffers::SEND_R][i];
		buffers.audio[Buffers::JACK_SIDECHAIN_KEY_L][i] = buffers.audio[Buffers::SIDECHAIN_KEY_L][i];
		buffers.audio[Buffers::JACK_SIDECHAIN_KEY_R][i] = buffers.audio[Buffers::SIDECHAIN_KEY_R][i];
		buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_L][i] = buffers.audio[Buffers::SIDECHAIN_SIGNAL_L][i];
		buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_R][i] = buffers.audio[Buffers::SIDECHAIN_SIGNAL_R][i];
	}


	/// db meter on master input & output
	inputMeter->process( nframes, buffers.audio[Buffers::MASTER_INPUT_L], buffers.audio[Buffers::MASTER_INPUT_R]);
	masterMeter->process(nframes, buffers.audio[Buffers::JACK_MASTER_OUT_L], buffers.audio[Buffers::JACK_MASTER_OUT_R] );

	if ( uiUpdateCounter > uiUpdateConstant ) {
		// instead of scaling whole buffer, just scale output by vol
		EventTrackSignalLevel e(-1, masterMeter->getLeftDB(), masterMeter->getRightDB() );
		writeToGuiRingbuffer( &e );
		EventTrackSignalLevel e2(-2, inputMeter->getLeftDB() * inputVolLag, inputMeter->getRightDB() * inputVol );
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
	if(lastnframes+nframes<buffers.nframes) {
		lastnframes=nframes;
		buffers.audio[Buffers::MASTER_INPUT_L]         = &buffers.audio[Buffers::MASTER_INPUT_L][nframes];
		buffers.audio[Buffers::MASTER_INPUT_R]         = &buffers.audio[Buffers::MASTER_INPUT_R][nframes];
		buffers.audio[Buffers::MASTER_RETURN_L]        = &buffers.audio[Buffers::MASTER_RETURN_L][nframes];
		buffers.audio[Buffers::MASTER_RETURN_R]        = &buffers.audio[Buffers::MASTER_RETURN_R][nframes];
		buffers.audio[Buffers::HEADPHONES_OUT_L]       = &buffers.audio[Buffers::HEADPHONES_OUT_L][nframes];
		buffers.audio[Buffers::HEADPHONES_OUT_R]       = &buffers.audio[Buffers::HEADPHONES_OUT_R][nframes];

		buffers.audio[Buffers::JACK_SEND_OUT_L]        = &buffers.audio[Buffers::JACK_SEND_OUT_L][nframes];
		buffers.audio[Buffers::JACK_SEND_OUT_R]        = &buffers.audio[Buffers::JACK_SEND_OUT_R][nframes];
		buffers.audio[Buffers::JACK_MASTER_OUT_L]      = &buffers.audio[Buffers::JACK_MASTER_OUT_L][nframes];
		buffers.audio[Buffers::JACK_MASTER_OUT_R]      = &buffers.audio[Buffers::JACK_MASTER_OUT_R][nframes];
		buffers.audio[Buffers::JACK_SIDECHAIN_KEY_L]   = &buffers.audio[Buffers::JACK_SIDECHAIN_KEY_L][nframes];
		buffers.audio[Buffers::JACK_SIDECHAIN_KEY_R]   = &buffers.audio[Buffers::JACK_SIDECHAIN_KEY_R][nframes];
		buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_L]= &buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_L][nframes];
		buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_R]= &buffers.audio[Buffers::JACK_SIDECHAIN_SIGNAL_R][nframes];
		if(gui->enablePerTrackOutput) {
			// Traverse tracks using the first track as offset
			for(int track=0; track<NTRACKS; track++) {
				// index = first-track + (track * channels)
				int trackoffset = track * NCHANNELS;
				buffers.audio[Buffers::JACK_TRACK_0_L+trackoffset] = &buffers.audio[Buffers::JACK_TRACK_0_L+trackoffset][nframes];
				buffers.audio[Buffers::JACK_TRACK_0_R+trackoffset] = &buffers.audio[Buffers::JACK_TRACK_0_R+trackoffset][nframes];
			}
		}
	} else
		lastnframes=0;

	return;
}

void Jack::clearInternalBuffers(int nframes)
{
	memset(buffers.audio[Buffers::SEND_L],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::SEND_R],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::SIDECHAIN_KEY_L],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::SIDECHAIN_KEY_R],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::SIDECHAIN_SIGNAL_L],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::SIDECHAIN_SIGNAL_R],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::MASTER_OUT_L],0,sizeof(float)*nframes);
	memset(buffers.audio[Buffers::MASTER_OUT_R],0,sizeof(float)*nframes);
	// Traverse tracks using the first track as offset
	for(int track=0; track<NTRACKS; track++) {
		// index = first-track + (track * channels)
		int trackoffset = track * NCHANNELS;
		memset(buffers.audio[Buffers::TRACK_0_L + trackoffset],0,sizeof(float)*nframes);
		memset(buffers.audio[Buffers::TRACK_0_R + trackoffset],0,sizeof(float)*nframes);
		memset(buffers.audio[Buffers::SEND_TRACK_0_L + trackoffset],0,sizeof(float)*nframes);
		memset(buffers.audio[Buffers::SEND_TRACK_0_R + trackoffset],0,sizeof(float)*nframes);
		memset(buffers.audio[Buffers::RETURN_TRACK_0_L + trackoffset],0,sizeof(float)*nframes);
		memset(buffers.audio[Buffers::RETURN_TRACK_0_R + trackoffset],0,sizeof(float)*nframes);
	}
}

void Jack::masterVolume(float vol)
{
	masterVol = vol;
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
	switch ( to ) {
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
	switch ( to ) {
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
