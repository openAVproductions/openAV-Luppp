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

#ifndef LUPPP_LOOPER_CLIP_H
#define LUPPP_LOOPER_CLIP_H

#include <stdio.h>
#include "state/stately.hxx"
#include "config.hxx"
#include "gridlogic.hxx"

#include "rubberband/RubberBandStretcher.h"

class AudioBuffer;

/** LooperClip
 * Represents each clip that a looper can playback. The core of the audio
 * samples is stored in AudioBuffer objects which are dynamically resized. The
 * base size of a AudioBuffer is 1 second's worth, after which larger buffers
 * will be requested.
 * The transition between AudioBuffer instances is seamless: when the clip is
 * running out, the new one is requested. Upon its arrival the current data is
 * copied, and the old buffer is returned for deallocation.
 *
 * This system allows for arbitrary length recordings, without huge
 * pre-allocated buffers while it is still quite simple.
 *
 * Each clip has its properties like length and bars/beats, so the Looper knows
 * to dynamically stretch / process the audio appropriately. Controllers and the
 * UI are updated from this data.
 *
 * This class inherits from Stately to save its state.
**/
class LooperClip : public Stately
{
public:
	LooperClip(int track, int scene);

	/// Set everything up
	void init();

	/// loads a sample: eg from disk, unloading current sample if necessary
	void load( AudioBuffer* ab );

	/// Return one Sample according to current playSpeed
	void getSample(long double playSpeed, float* L, float* R);
	/// Record $count Samples into internal Buffer
	void
	getSamples(unsigned int nframes, long double playspeed, float * L, float * R);
	void record(int count, float* L, float* R);

	/// TimeObserver override
	void bar();

	/// Start process of saving the clip
	void save();
	/// Reset the clip to initial state
	void reset();

	/// analyses current _playing _recording vars, returns the current State
	GridLogic::State getState();
	bool playing();
	bool getLoaded();
	bool getQueueStop();
	bool getQueuePlay();
	bool recording();

	/// Get number of Beats in the Buffer
	int   getBeats();
	/// Get playback progress
	float getProgress();
	/// Get position of the playhead
	float getPlayhead();

	/// Return the length of the complete buffer
	long  getBufferLenght();
	/// Return the nr of samples holding actual audio. This is less then getBufferLength();
	long  getActualAudioLength();
	/// Return Size of the Buffer
	size_t audioBufferSize();
	int
	getRecFpb() {
		return _recFpb;
	};

	/// Queue Play
	void  queuePlay();
	/// Queue Stop
	void  queueStop();
	/// Queue Record
	void  queueRecord();

	// removes all queued States
	void  resetQueues(); 
	// returns true if any state is queued
	bool  somethingQueued(); 

	/// set buffer state
	void setBeats(int beats);

	/** used to update the size of the buffer for this looperclip. The current
	 *  data is copied into the new buffer, then the smaller buffer is sent
	 *  for de-allocation.
	**/
	void setRequestedBuffer( AudioBuffer* ab );

	/// used for saving the contents of this buffer to disk
	void recieveSaveBuffer( AudioBuffer* ab );

	/// reset the play head to zero. Does nothing when recording
	void resetPlayHead();

#ifdef BUILD_TESTS
	// used only in test cases
	void setState( bool load, bool play, bool rec, bool qPlay, bool qStop, bool qRec );
#endif

private:
	int track, scene;

	/** Luppp needs more than the current state of the clip to accuratly handle
	 *  it. The current state of the grid is kept up-to-date by GridLogic
	 *  abstracting detail away, sending GridLogic::State to Controllers.
	**/
	bool _loaded;
	bool _playing;
	bool _recording;

	bool _queuePlay;
	bool _queueStop;
	bool _queueRecord;

	bool _newBufferInTransit;

	long double _playhead;
	float _recordhead;
	int _recFpb;

	float _smoothVolume;

	unsigned int _barsPlayed;
	AudioBuffer* _buffer;

	/// Request new internal Buffer
	void requestNewBuffer();
	/// Get if a new internal Buffer is on the way
	bool newBufferInTransit();
	/// Get available Space to record
	unsigned long recordSpaceAvailable();

	/// Change State to Playing
	void setPlaying();
	/// Change State to Recording
	void setRecording();
	/// Change State to Stopped
	void setStopped();

	/// Updates all the controllers with the current state
	void updateController();
	RubberBand::RubberBandStretcher *stretcher;
};

#endif // LUPPP_LOOPER_CLIP_H

