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

#ifndef LUPPP_AUDIO_ENGINE_H
#define LUPPP_AUDIO_ENGINE_H

#include <string>
#include <vector>
#include "event.hxx"

class State;
class Logic;
class Looper;
class Metronome;
class GridLogic;
class TimeManager;
class ControllerUpdater;
class TrackOutput;

/** AudioEngine
  Abstract base class for audio backends (JACK, RtAudio, etc.)
  Provides common interface for audio I/O operations.
**/
class AudioEngine
{
public:
	virtual ~AudioEngine() {}

	/// Factory method to create appropriate audio engine based on backend type
	static AudioEngine* create(std::string name, std::string backend = "jack");

	/// Activate the audio engine
	virtual void activate() = 0;

	/// Quit the audio engine, destroying ports etc. Call only on exit.
	virtual void quit() = 0;

	/// Smoothing value for parameter interpolation
	float smoothing_value;

	/// Get buffer size in frames
	virtual int getBuffersize() = 0;

	/// Get sample rate in Hz
	virtual int getSamplerate() = 0;

	/// Process audio frames
	virtual void processFrames(int nframes) = 0;

	/// Clear internal buffers
	virtual void clearInternalBuffers(int nframes) = 0;

	/// Get components
	virtual Looper* getLooper(int track) = 0;
	virtual TrackOutput* getTrackOutput(int track) = 0;
	virtual State* getState() = 0;
	virtual Logic* getLogic() = 0;
	virtual Metronome* getMetronome() = 0;
	virtual GridLogic* getGridLogic() = 0;
	virtual TimeManager* getTimeManager() = 0;
	virtual ControllerUpdater* getControllerUpdater() = 0;

	// Audio volume methods
	virtual float getInputVolume() = 0;
	virtual void inputVolume(float vol) = 0;
	virtual void masterVolume(float vol) = 0;
	virtual void returnVolume(float vol) = 0;
	virtual void inputTo(Event::INPUT_TO to, float v) = 0;
	virtual void inputToActive(Event::INPUT_TO to, bool active) = 0;

	// MIDI binding state
	virtual void resetMidiBindingState() = 0;

	/// Transport methods
	virtual void transportRolling(bool rolling) = 0;
	virtual void transportState(bool rolling) = 0;

	/// MIDI binding
	virtual void midiBindingEnable(int eventType, int track, int scene, int send, int active) = 0;
	virtual void midiBindingDisable() = 0;

	/// Register MIDI controller
	virtual void registerMidiIO(class MidiIO* mio) = 0;
	virtual void unregisterMidiIO(class MidiIO* mio) = 0;
};

#ifdef AUDIO_BACKEND_RTAUDIO

#include "rtaudiomidi/RtAudio.h"
#include "rtaudiomidi/RtMidi.h"

/** RtAudioEngine
  RtAudio-based audio backend for cross-platform audio I/O.
  Supports CoreAudio (macOS), ALSA (Linux), WASAPI (Windows), etc.
**/
class RtAudioEngine : public AudioEngine
{
public:
	RtAudioEngine(std::string name);
	virtual ~RtAudioEngine();

	virtual void activate() override;
	virtual void quit() override;

	virtual int getBuffersize() override;
	virtual int getSamplerate() override;

	virtual void processFrames(int nframes) override;
	virtual void clearInternalBuffers(int nframes) override;

	virtual Looper* getLooper(int track) override;
	virtual TrackOutput* getTrackOutput(int track) override;

	virtual State* getState() override { return state; }
	virtual Logic* getLogic() override { return logic; }
	virtual Metronome* getMetronome() override { return metronome; }
	virtual GridLogic* getGridLogic() override { return gridLogic; }
	virtual TimeManager* getTimeManager() override { return timeManager; }
	virtual ControllerUpdater* getControllerUpdater() override { return controllerUpdater; }

	virtual float getInputVolume() override;
	virtual void inputVolume(float vol) override;
	virtual void masterVolume(float vol) override;
	virtual void returnVolume(float vol) override;
	virtual void inputTo(Event::INPUT_TO to, float v) override;
	virtual void inputToActive(Event::INPUT_TO to, bool active) override;

	virtual void resetMidiBindingState() override;

	virtual void transportRolling(bool rolling) override;
	virtual void transportState(bool rolling) override;

	virtual void midiBindingEnable(int eventType, int track, int scene, int send, int active) override;
	virtual void midiBindingDisable() override;

	virtual void registerMidiIO(class MidiIO* mio) override;
	virtual void unregisterMidiIO(class MidiIO* mio) override;

private:
	RtAudio audioDevice;
	unsigned int bufferFrames;
	unsigned int sampleRate;
	bool isActive;

	// Core components (same as Jack)
	State* state;
	Logic* logic;
	Metronome* metronome;
	GridLogic* gridLogic;
	TimeManager* timeManager;
	ControllerUpdater* controllerUpdater;

	std::vector<Looper*> loopers;
	std::vector<TrackOutput*> trackOutputs;
	std::vector<class MidiIO*> midiIO;

	// Audio volume control
	float inputVol;

	// Internal audio buffers
	std::vector<float> masterInputL;
	std::vector<float> masterInputR;
	std::vector<float> masterOutputL;
	std::vector<float> masterOutputR;
	std::vector<float> headphonesL;
	std::vector<float> headphonesR;

	// MIDI binding state
	bool bindingEventRecordEnable;
	int bindingEventType;
	int bindingTrack;
	int bindingScene;
	int bindingSend;
	int bindingActive;

	// Static callback for RtAudio
	static int rtAudioCallback(void* outputBuffer, void* inputBuffer,
	                          unsigned int nFrames, double streamTime,
	                          RtAudioStreamStatus status, void* userData);

	// Instance callback
	int processAudio(void* outputBuffer, void* inputBuffer, unsigned int nFrames);
};

#endif // AUDIO_BACKEND_RTAUDIO

#endif // LUPPP_AUDIO_ENGINE_H
