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

#ifndef LUPPP_AUDIOBUFFER_H
#define LUPPP_AUDIOBUFFER_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>

using namespace std;

/// AudioBuffer stores float samples in a big vector.
class AudioBuffer
{
public:
	AudioBuffer();
	AudioBuffer(unsigned long size);

	void init();

	/// this function is used for "resizing" an exisiting buffer, and should
	/// not be called for any other reason.
	void setID(int id);

	int getID();
	void setName(const char* n);

	char* getName();
	int getBeats();

	void setBeats(int b);

	void setAudioFrames(long af);

	long getAudioFrames();

    long getSize();

	std::vector<float>& getDataL();
	std::vector<float>& getDataR();

	void nonRtSetSample(std::vector<float>& sampleL, std::vector<float>& sampleR);

	void nonRtResize(unsigned long size);

	//friend ostream& operator<<(ostream& o, const AudioBuffer& a);

protected:
	static int privateID;
	int ID;

	int numBeats;

	/// holds the number of samples that are usable audio, as opposed to
	/// buffer.size(), which also has non-used space at the end.
	long audioFrames;

	char name[20];

	std::vector<float> bufferL;
	std::vector<float> bufferR;
};

#endif

