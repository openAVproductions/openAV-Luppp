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

#include "audioengine.hxx"
#include "debug.hxx"
#include "event.hxx"

#ifdef AUDIO_BACKEND_JACK
#include "jack.hxx"
#endif

#ifdef AUDIO_BACKEND_RTAUDIO

#include <cstring>
#include <iostream>

#include "state/state.hxx"
#include "logic.hxx"
#include "config.hxx"
#include "looper.hxx"
#include "metronome.hxx"
#include "gridlogic.hxx"
#include "trackoutput.hxx"
#include "timemanager.hxx"
#include "controllerupdater.hxx"
#include "audiobuffer.hxx"
#include "buffers.hxx"
#include "event.hxx"

RtAudioEngine::RtAudioEngine(std::string name)
	: bufferFrames(512)
	, sampleRate(44100)
	, isActive(false)
	, state(new State())
	, controllerUpdater(new ControllerUpdater())
{
	LUPPP_NOTE("RtAudioEngine: Initializing with name: %s", name.c_str());

	// Initialize core components
	timeManager = new TimeManager();
	timeManager->setSamplerate(sampleRate);
	
	metronome = new Metronome();
	logic = new Logic();
	gridLogic = new GridLogic();

	// Manually register TimeObservers since jack pointer is null
	timeManager->registerObserver(metronome);
	timeManager->registerObserver(gridLogic);

	resetMidiBindingState();

	// Reserve space for MIDI controllers
	midiIO.reserve(CONTROLLERS_PREALLOC);

	// Try to find available audio device
	if (audioDevice.getDeviceCount() < 1) {
		LUPPP_ERROR("RtAudioEngine: No audio devices found!");
		return;
	}

	// Get default output device
	RtAudio::DeviceInfo info = audioDevice.getDeviceInfo(audioDevice.getDefaultOutputDevice());
	LUPPP_NOTE("RtAudioEngine: Using device: %s", info.name.c_str());

	// Update sample rate from device
	if (!info.sampleRates.empty()) {
		// Prefer 44100 or 48000
		bool found = false;
		for (unsigned int rate : info.sampleRates) {
			if (rate == 44100 || rate == 48000) {
				sampleRate = rate;
				found = true;
				break;
			}
		}
		if (!found) {
			sampleRate = info.sampleRates[0];
		}
	}

	LUPPP_NOTE("RtAudioEngine: Sample rate: %d Hz, Buffer size: %d frames", 
	           sampleRate, bufferFrames);

	// Initialize audio volume
	inputVol = 1.0f;

	// Allocate internal buffers
	masterInputL.resize(bufferFrames, 0.0f);
	masterInputR.resize(bufferFrames, 0.0f);
	masterOutputL.resize(bufferFrames, 0.0f);
	masterOutputR.resize(bufferFrames, 0.0f);
	headphonesL.resize(bufferFrames, 0.0f);
	headphonesR.resize(bufferFrames, 0.0f);

	// Setup tracks (8 tracks)
	for (int track = 0; track < NTRACKS; track++) {
		loopers.push_back(new Looper(track));
		trackOutputs.push_back(new TrackOutput(track, loopers.back()));
		
		// Manually register each Looper as TimeObserver
		timeManager->registerObserver(loopers.back());
	}
}

RtAudioEngine::~RtAudioEngine()
{
	quit();

	// Clean up tracks
	for (auto looper : loopers) {
		delete looper;
	}
	for (auto trackOutput : trackOutputs) {
		delete trackOutput;
	}

	delete controllerUpdater;
	delete timeManager;
	delete gridLogic;
	delete logic;
	delete metronome;
	delete state;
}

void RtAudioEngine::activate()
{
	if (isActive) {
		LUPPP_WARN("RtAudioEngine: Already active!");
		return;
	}

	RtAudio::StreamParameters outputParams;
	outputParams.deviceId = audioDevice.getDefaultOutputDevice();
	outputParams.nChannels = 2;
	outputParams.firstChannel = 0;

	RtAudio::StreamParameters inputParams;
	inputParams.deviceId = audioDevice.getDefaultInputDevice();
	inputParams.nChannels = 2;
	inputParams.firstChannel = 0;

	RtAudio::StreamOptions options;
	options.flags = RTAUDIO_MINIMIZE_LATENCY;
	options.numberOfBuffers = 2;

	try {
		audioDevice.openStream(&outputParams,
		                       &inputParams,
		                       RTAUDIO_FLOAT32,
		                       sampleRate,
		                       &bufferFrames,
		                       &RtAudioEngine::rtAudioCallback,
		                       this,
		                       &options);

		audioDevice.startStream();
		isActive = true;

		LUPPP_NOTE("RtAudioEngine: Activated successfully");
		LUPPP_NOTE("  Actual buffer size: %d frames", bufferFrames);
	} catch (RtAudioErrorType& e) {
		LUPPP_ERROR("RtAudioEngine: Error opening audio stream: %d", e);
	}
}

void RtAudioEngine::quit()
{
	if (audioDevice.isStreamRunning()) {
		try {
			audioDevice.stopStream();
		} catch (RtAudioErrorType& e) {
			LUPPP_ERROR("RtAudioEngine: Error stopping stream: %d", e);
		}
	}

	if (audioDevice.isStreamOpen()) {
		audioDevice.closeStream();
	}

	isActive = false;
	LUPPP_NOTE("RtAudioEngine: Quit");
}

int RtAudioEngine::getBuffersize()
{
	return static_cast<int>(bufferFrames);
}

int RtAudioEngine::getSamplerate()
{
	return static_cast<int>(sampleRate);
}

void RtAudioEngine::processFrames(int nframes)
{
	// This is called from the audio callback
	// Process all audio logic here
	
	// Clear output buffers
	std::memset(masterOutputL.data(), 0, nframes * sizeof(float));
	std::memset(masterOutputR.data(), 0, nframes * sizeof(float));

	// Note: Full audio processing with Buffers structure would go here
	// This is a simplified implementation for initial RtAudio support
	// Time management would need a Buffers structure:
	// if (timeManager) {
	//     timeManager->process(&buffers);
	// }

	// Process each track
	for (size_t i = 0; i < trackOutputs.size(); i++) {
		if (trackOutputs[i]) {
			// Track processing would go here
			// trackOutputs[i]->process(nframes, &buffers);
		}
	}

	// Mix to master output (simplified for now)
	// In full implementation, this would mix all tracks
}

void RtAudioEngine::clearInternalBuffers(int nframes)
{
	if (nframes > static_cast<int>(bufferFrames)) {
		nframes = bufferFrames;
	}

	std::memset(masterOutputL.data(), 0, nframes * sizeof(float));
	std::memset(masterOutputR.data(), 0, nframes * sizeof(float));
	std::memset(headphonesL.data(), 0, nframes * sizeof(float));
	std::memset(headphonesR.data(), 0, nframes * sizeof(float));
}

float RtAudioEngine::getInputVolume()
{
	return inputVol;
}

void RtAudioEngine::inputVolume(float vol)
{
	inputVol = vol;
}

void RtAudioEngine::masterVolume(float vol)
{
	// TODO: Implement master volume control for RtAudio
	LUPPP_NOTE("RtAudioEngine::masterVolume(%f) - not yet implemented", vol);
}

void RtAudioEngine::returnVolume(float vol)
{
	// TODO: Implement return volume control for RtAudio
	LUPPP_NOTE("RtAudioEngine::returnVolume(%f) - not yet implemented", vol);
}

void RtAudioEngine::inputTo(Event::INPUT_TO to, float v)
{
	// TODO: Implement input routing for RtAudio
	LUPPP_NOTE("RtAudioEngine::inputTo(%d, %f) - not yet implemented", to, v);
}

void RtAudioEngine::inputToActive(Event::INPUT_TO to, bool active)
{
	// TODO: Implement input routing activation for RtAudio
	LUPPP_NOTE("RtAudioEngine::inputToActive(%d, %d) - not yet implemented", to, active);
}

Looper* RtAudioEngine::getLooper(int track)
{
	if (track >= 0 && track < static_cast<int>(loopers.size())) {
		return loopers[track];
	}
	return nullptr;
}

TrackOutput* RtAudioEngine::getTrackOutput(int track)
{
	if (track >= 0 && track < static_cast<int>(trackOutputs.size())) {
		return trackOutputs[track];
	}
	return nullptr;
}

void RtAudioEngine::transportRolling(bool rolling)
{
	if (timeManager) {
		timeManager->setTransportState(rolling ? TRANSPORT_ROLLING : TRANSPORT_STOPPED);
	}
}

void RtAudioEngine::transportState(bool rolling)
{
	transportRolling(rolling);
}

void RtAudioEngine::midiBindingEnable(int eventType, int track, int scene, int send, int active)
{
	bindingEventRecordEnable = true;
	bindingEventType = eventType;
	bindingTrack = track;
	bindingScene = scene;
	bindingSend = send;
	bindingActive = active;
}

void RtAudioEngine::midiBindingDisable()
{
	resetMidiBindingState();
}

void RtAudioEngine::resetMidiBindingState()
{
	bindingEventRecordEnable = false;
	bindingEventType = 0; // EVENT_NULL
	bindingTrack = -2;
	bindingScene = -1;
	bindingSend = -1;
	bindingActive = 1;
}

void RtAudioEngine::registerMidiIO(MidiIO* mio)
{
	midiIO.push_back(mio);
	LUPPP_NOTE("RtAudioEngine: Registered MIDI I/O");
}

void RtAudioEngine::unregisterMidiIO(MidiIO* mio)
{
	for (auto it = midiIO.begin(); it != midiIO.end(); ++it) {
		if (*it == mio) {
			midiIO.erase(it);
			LUPPP_NOTE("RtAudioEngine: Unregistered MIDI I/O");
			return;
		}
	}
}

int RtAudioEngine::rtAudioCallback(void* outputBuffer, void* inputBuffer,
                                   unsigned int nFrames, double streamTime,
                                   RtAudioStreamStatus status, void* userData)
{
	RtAudioEngine* engine = static_cast<RtAudioEngine*>(userData);
	return engine->processAudio(outputBuffer, inputBuffer, nFrames);
}

int RtAudioEngine::processAudio(void* outputBuffer, void* inputBuffer, unsigned int nFrames)
{
	float* output = static_cast<float*>(outputBuffer);
	float* input = static_cast<float*>(inputBuffer);

	// Copy input to internal buffers (interleaved to separate channels)
	if (input) {
		for (unsigned int i = 0; i < nFrames; i++) {
			masterInputL[i] = input[i * 2];
			masterInputR[i] = input[i * 2 + 1];
		}
	}

	// Process audio
	processFrames(nFrames);

	// Copy internal buffers to output (separate channels to interleaved)
	if (output) {
		for (unsigned int i = 0; i < nFrames; i++) {
			output[i * 2] = masterOutputL[i];
			output[i * 2 + 1] = masterOutputR[i];
		}
	}

	return 0; // Continue stream
}

#endif // AUDIO_BACKEND_RTAUDIO

// Factory implementation
AudioEngine* AudioEngine::create(std::string name, std::string backend)
{
	LUPPP_NOTE("AudioEngine::create - backend: %s", backend.c_str());

#ifdef AUDIO_BACKEND_JACK
	if (backend == "jack") {
		return new Jack(name);
	}
#endif

#ifdef AUDIO_BACKEND_RTAUDIO
	if (backend == "rtaudio") {
		return new RtAudioEngine(name);
	}
#endif

	LUPPP_ERROR("AudioEngine::create - Unknown backend: %s", backend.c_str());
	return nullptr;
}
