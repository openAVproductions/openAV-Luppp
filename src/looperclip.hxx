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
 * This class inherits from SaveAble to save its state.
**/
class LooperClip : public Stately
{
public:
	LooperClip(int track, int scene);

	void init();

	/// loads a sample: eg from disk, unloading current sample if necessary
	void load( AudioBuffer* ab );

	/// audio functionality
	void getSample(float playSpeed, float* L, float* R);
	void record(int count, float* L, float* R);

	/// TimeObserver override
	void bar();

	/// SaveAble overrides
	void save();
	void reset();

	/// analyses current _playing _recording vars, returns the current State
	GridLogic::State getState();
	bool playing();
	bool getLoaded();
	bool getQueueStop();
	bool getQueuePlay();
	bool recording();

	/// get buffer details
	int   getBeats();
	float getProgress();
	float getPlayhead();
	//Return the length of the complete buffer
	long  getBufferLenght();
	//Return the nr of samples holding actual audio. This is less then getBufferLength();
	long  getActualAudioLength();
	size_t audioBufferSize();

	AudioBuffer* getAudioBuffer()
	{
		return _buffer;
	}

	/// set clip state
	void  queuePlay(bool=true);
	void  queueStop();
	void  queueRecord();

	void  neutralize(); // removes all play || record states if on
	bool  somethingQueued(); // returns true if any state is queued

	/// set buffer state
	void setBeats(int beats);

	/// Luppp internal buffer resizing
	void newBufferInTransit(bool n);
	bool newBufferInTransit();
	unsigned long recordSpaceAvailable();

	/** used to update the size of the buffer for this looperclip. The current
	 *  data is copied into the new buffer, then the smaller buffer is sent
	 *  for de-allocation.
	**/
	void setRequestedBuffer( AudioBuffer* ab );

	/// used for saving the contents of this buffer to disk
	void recieveSaveBuffer( AudioBuffer* ab );

	///reset the play head to zero. Does nothing when recording
	void setPlayHead(float ph);

    void setWantedBeats(int beats);

    int getWantedBeats();

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

	float _playhead;
	float _recordhead;
    int _wantedBeats;
	AudioBuffer* _buffer;
};

#endif // LUPPP_LOOPER_CLIP_H

