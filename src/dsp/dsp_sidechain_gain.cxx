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

#include "dsp_sidechain_gain.hxx"

// for pow()
#include <cmath>

SidechainGain::SidechainGain(int rate) :
	/// initial control values
	controlThreshold(0.2),
	controlReduction(1),
	controlReleaseTime(0.5),

	/// filter state init
	w(10.0f / (rate * 0.02)),
	a(0.07f),
	b(1.0f / (1.0f - a)),
	g1(0.0f),
	g2(0.0f),


	peakFrameCounter(0),
	peakCountDuration( rate / 4 ),
	currentTarget(0)
{
}

void SidechainGain::process(unsigned int n_samples, float** inputs, float** outputs)
{
	/// audio inputs
	float* inL  = inputs[0];
	float* inR  = inputs[1];
	float* side = inputs[2];
	float* outL = outputs[0];
	float* outR = outputs[1];

	/// control inputs
	float threshold   = controlThreshold;
	float reduction   = controlReduction;
	float releaseTime = controlReleaseTime;


	/// analyse sidechain input for peak
	float sum = 0.f;
	for( unsigned int i = 0; i < n_samples; i++ ) {
		if ( *side > 0.000001 )
			sum += *side++;
		else
			sum += -*side++;
	}

	currentTarget = 0.f;

	/// check for peak level (offset to avoid "always on" peak)
	if ( sum / n_samples > threshold + 0.05 ) {
		peakFrameCounter = peakCountDuration * releaseTime;
		currentTarget = 1.f - reduction;
	} else if ( peakFrameCounter < 0 ) {
		currentTarget = 1.f;
	} else {
		currentTarget = 1.f - reduction;
	}

	if ( currentTarget < 0.f )
		currentTarget = 0.f;

	peakFrameCounter -= n_samples;

	for( unsigned int i = 0; i < n_samples; i++ ) {
		/// smoothing algo is a lowpass, to de-zip the fades
		/// x^^4 approximates linear volume increase for human ears
		g1 += w * ( pow ( currentTarget, 4.f ) - g1 - a * g2 - 1e-20f);
		g2 += w * (b * g1 - g2 + 1e-20f);
		float gain = g2;

		*outL++ = *inL++ * gain;
		*outR++ = *inR++ * gain;
	}

	/// update output value
	controlSidechainAmp = currentTarget;
}
