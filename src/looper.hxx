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

#ifndef LUPPP_LOOPER_H
#define LUPPP_LOOPER_H

#include <vector>
#include <iostream>

#include "buffers.hxx"

#include "audioprocessor.hxx"

#include "looperclip.hxx"
#include "observer/time.hxx"

using namespace std;

/** Looper
 * The class which reads from LooperClips, and reads/ writes the data using the
 * track buffer. Scene recording / playback is the essential functionality here.
**/
class Looper : public AudioProcessor, public TimeObserver
{
  public:
    Looper(int t);
    
    /// *sets* the new audiobuffer, but the content gets copied to the new buffer.
    /// Used for infinite lenght recording
    void setRequestedBuffer(int s, AudioBuffer* ab);
    
    /// stores the framesPerBeat from TimeManager. Used to stretch audio
    void setFpb(int f);
    
    /// Retrieve a clip from the Looper
    LooperClip* getClip(int scene);
    
    /// Process nframes of audio
    void process(unsigned int nframes, Buffers* buffers);
  
  private:
    const int track;
    
    /// variables used to determing the current actions of the looper
    int playingScene;
    int queuedScene;
    
    int fpb;
    
    //vector<float> tmpRecordBuffer;
    LooperClip* clips[10];
    
    // Pitch Shifting
    void pitchShift(int count, float* input, float* output);
    vector<float> tmpBuffer;
    
    int    IOTA;
    float  fVec0[65536];
    float  semitoneShift;
    float  windowSize;
    float  fRec0[2];
    float  crossfadeSize;
    float  fSamplingFreq;
    
    int uiUpdateConstant;
    int uiUpdateCounter;
};

#endif // LUPPP_LOOPER_H
