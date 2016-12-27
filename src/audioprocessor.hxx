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

#ifndef LUPPP_AUDIO_PROCESSOR_H
#define LUPPP_AUDIO_PROCESSOR_H

#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

class AudioProcessor
{
public:
	AudioProcessor() {}

	/// copies the track output to master buffer, sidechain & post-side buffer
	virtual void process(unsigned int nframes, Buffers* buffers)
	{
		printf("AudioProcessor::process() not derived\n");
	}

	/// set main mix, 0-1
	virtual void setMaster(float value) {}

	/// set sidechain mix, 0-1
	virtual void setSidechain(float value) {}

	/// set post sidechain mix, 0-1
	virtual void setPostSidechain(float value) {}

	/// set reverb mix, 0-1
	virtual void setReverb(float value) {}

	virtual ~AudioProcessor() {};
};

#endif // LUPPP_AUDIO_PROCESSOR_H

