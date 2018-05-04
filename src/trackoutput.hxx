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

#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

#include <iostream>
#include <stdio.h>

#include "buffers.hxx"
#include "config.hxx"
#include "audioprocessor.hxx"

#include "eventhandler.hxx"
#include "dsp/dsp_dbmeter.hxx"

/** TrackOutput
 *  This class is used to mix down each track, and produce output: that output
 *  exists in the master outputs and send buffers.
**/
class TrackOutput : public AudioProcessor
{
public:
	TrackOutput(int t, AudioProcessor* ap);

	/// set main mix, 0-1
	void setMaster(float value);

	float getMaster();

	// panning
	void setPan ( float pan );

	bool recordArm();
	void recordArm(bool r);

	/// set send
	void setSend( int send, float value );
	void setSendActive( int send, bool active );

	/// copies the track output to master buffer, sidechain & post-side buffer
	void process(unsigned int nframes, Buffers* buffers);

	~TrackOutput();

private:
	int track;

	bool  _recordArm;
	/// _toMasterLag is a volume that lags behind _toMaster when setMaster() is called
	/// This prohibits audible jumps when rapidly changing the volume
	float _toMaster;
	float _toMasterLag;

	/* -1 indicates 100% left, 0 is centered, 1 indicates right */
	float _panL;
	float _panR;
	float _panLLag;
	float _panRLag;

	float _toSend;
	float _toSendLag;
	
	float _toSidechain;
	float _toPostSidechain;

	bool _toPostfaderActive;
	bool _toKeyActive;
	bool _toXSideActive;

	/// Pointer to "previous" processor: the graph is backwards
	AudioProcessor* previousInChain;

	// Metering variables
	long uiUpdateCounter;
	long uiUpdateConstant;
	DBMeter* dbMeter;

};

#endif // LUPPP_TRACK_OUTPUT_H

