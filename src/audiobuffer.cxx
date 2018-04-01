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

#include "audiobuffer.hxx"
#include "config.hxx"

#include <stdio.h>

AudioBuffer::AudioBuffer()
{
	ID = privateID++;
	init();
}

AudioBuffer::AudioBuffer(unsigned long size)
{
	// FIXME recorded buffers don't get an ID, using garbage IDs
	/// no ID assigned: it *takes* the one from the previous buffer!
	init();
	bufferL.resize(size);
	bufferR.resize(size);
}

void AudioBuffer::init()
{
	numBeats = 0;
	audioFrames = 0;
}

/// this function is used for "resizing" an exisiting buffer, and should
/// not be called for any other reason.
void AudioBuffer::setID(int id)
{
	ID = id;
}

int AudioBuffer::getID()
{
	return ID;
}

void AudioBuffer::setName(const std::string& n)
{
	name = n;
	if (n.size() > 20) {
#ifdef DEBUG_BUFFER
		cout << "AudioBuffer setName too long, truncating." << endl;
#endif
		name.resize(20);
	}
}

const std::string& AudioBuffer::getName() const
{
	return name;
}

int AudioBuffer::getBeats()
{
	return numBeats;
}

void AudioBuffer::setBeats(int b)
{
#ifdef DEBUG_BUFFER
	cout << "AudioBuffer now has " << b << " beats\n" << endl;
#endif
	numBeats = b;
}

void AudioBuffer::setAudioFrames(long af)
{
	audioFrames = af;
#ifdef DEBUG_BUFFER
	cout << "AudioBuffer " << ID << " has "  << audioFrames << " audioFrames\n" << endl;
#endif
}

long AudioBuffer::getAudioFrames()
{
	return audioFrames;
}

long AudioBuffer::getSize()
{
	if(bufferL.size() != bufferR.size()) {
		LUPPP_WARN("left and right channels of audio buffer have different size: %i vs %i", bufferL.size(), bufferR.size() );
	}
	return std::min(bufferL.size(), bufferR.size());
}

std::vector<float>& AudioBuffer::getDataL()
{
	return bufferL;
}
std::vector<float>& AudioBuffer::getDataR()
{
	return bufferR;
}

void AudioBuffer::nonRtSetSample(std::vector<float>& sampleL, std::vector<float>& sampleR)
{
	bufferL.swap(sampleL);
	bufferR.swap(sampleR);
}
void AudioBuffer::nonRtResize(unsigned long size)
{
	bufferL.resize(size);
	bufferR.resize(size);
}

/*
ostream& AudioBuffer::operator<<(ostream& o, const AudioBuffer& a)
{
  o << "AudioBuffer " << a.name <<
        " beats: " << a.numBeats <<
        " audioFrames: " << a.audioFrames << endl;
  return o;
}
*/
