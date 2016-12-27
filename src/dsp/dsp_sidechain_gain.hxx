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

#ifndef OPENAV_DSP_SIDECHAIN_GAIN_H
#define OPENAV_DSP_SIDECHAIN_GAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class SidechainGain
{
public:
	SidechainGain(int rate);
	~SidechainGain() {}

	void threshold(float t)
	{
		controlThreshold = t;
	}
	void reduction(float r)
	{
		controlReduction = r;
	}
	void time     (float t)
	{
		controlReleaseTime = t;
	}

	void process(unsigned int n_samples, float** inputs, float** outputs);

private:
	/// audio buffers
	float* audioInputL;
	float* audioInputR;
	float* audioSidechain;
	float* audioOutputL;
	float* audioOutputR;

	/// control signals
	float controlThreshold;
	float controlReduction;
	float controlReleaseTime;
	float controlSidechainAmp;

	/// filter state
	float w, a, b, g1, g2;

	/// last peak history
	bool nowIsAPeak;
	long peakFrameCounter;

	/// nframes available for countdown
	long peakCountDuration;

	/// control output
	float currentTarget;
};

#endif // OPENAV_DSP_SIDECHAIN_GAIN_H
