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

#include "trackoutput.hxx"

#include "jack.hxx"
extern Jack* jack;

TrackOutput::TrackOutput(int t, AudioProcessor* ap) :
	AudioProcessor(),
	track(t),
	_recordArm(false),
	previousInChain(ap)
{
	// UI update
	uiUpdateConstant = jack->getSamplerate() / 30;
	uiUpdateCounter  = jack->getSamplerate() / 30;

	dbMeter = new DBMeter( jack->getSamplerate() );

	_toMaster        = 0.8;
	_toMasterLag     = 0.8;

	_panL = 1.0f;
	_panR = 1.0f;
	_panLLag = 1.0f;
	_panRLag = 1.0f;

	_toSend = 0.0;
	_toSendLag = 0.0;

	// TODO this is unused and can be removed
	_toSidechain     = 0.0;

	_toPostSidechain    = 0.0;
	_toPostSidechainLag = 0.0;

	_toPostfaderActive        = 0;
	_toKeyActive     = 0;
	_toXSideActive = true;
}


void TrackOutput::setMaster(float value)
{
	if(value < 0.01)
		value = 0.f;
	_toMaster = value;
}

float TrackOutput::getMaster()
{
	return _toMaster;
}


bool TrackOutput::recordArm()
{
	return _recordArm;
}

void TrackOutput::recordArm(bool r)
{
	_recordArm = r;
}

void TrackOutput::setSendActive( int send, bool a )
{
	switch( send ) {
	case SEND_POSTFADER:
		_toPostfaderActive = a;
		break;
	case SEND_KEY:
		_toKeyActive = a;
		break;
	//case SEND_XSIDE:
	//    _toXSideActive = a;
	default:
		break;
	}
}

void TrackOutput::setPan( float pan )
{
	/* Trial + Error leads to this algo - its cheap and cheerful */
	if (pan <= 0)
	{
		// pan to left channel, lower right one
		_panR = pan + 1.0f;
	}
	else
	{
		// pan to right channel, lower left one
		_panL = (pan * -1) + 1.0f;
	}
}

void TrackOutput::setSend( int send, float value )
{
	switch( send ) {
	case SEND_POSTFADER:
		_toSend = value;
		break;
	case SEND_KEY:
		// setSendActive() handles on/off for this send
		//_toSidechain = value;
		break;
	case SEND_XSIDE:
		_toPostSidechain = value;
		break;
	}
}

void TrackOutput::process(unsigned int nframes, Buffers* buffers)
{
	// index = first-track + (track * channels)
	int trackoffset = track * NCHANNELS;

	//compute master volume lag;
	_toMasterLag += SMOOTHING_CONST * (_toMaster - _toMasterLag);

	// get & zero track buffer
	float* trackBufferL = buffers->audio[Buffers::RETURN_TRACK_0_L + trackoffset];
	float* trackBufferR = buffers->audio[Buffers::RETURN_TRACK_0_R + trackoffset];
	memset( trackBufferL, 0, sizeof(float)*nframes );
	memset( trackBufferR, 0, sizeof(float)*nframes );

	// call process() up the chain
	previousInChain->process( nframes, buffers );

	// run the meter
	dbMeter->process( nframes, trackBufferL, trackBufferR );

	if (uiUpdateCounter > uiUpdateConstant ) {
		float l = dbMeter->getLeftDB() * _toMasterLag;
		float r = dbMeter->getRightDB() * _toMasterLag;
		EventTrackSignalLevel e( track, l, r );
		writeToGuiRingbuffer( &e );
		uiUpdateCounter = 0;
	}

	uiUpdateCounter += nframes;

	// copy audio data into send / sidechain / master buffers
	float* sendL          = buffers->audio[Buffers::SEND_L];
	float* sendR          = buffers->audio[Buffers::SEND_R];
	float* sidechainL     = buffers->audio[Buffers::SIDECHAIN_KEY_L];
	float* sidechainR     = buffers->audio[Buffers::SIDECHAIN_KEY_R];
	float* postSidechainL = buffers->audio[Buffers::SIDECHAIN_SIGNAL_L];
	float* postSidechainR = buffers->audio[Buffers::SIDECHAIN_SIGNAL_R];

	float* masterL       = buffers->audio[Buffers::MASTER_OUT_L];
	float* masterR       = buffers->audio[Buffers::MASTER_OUT_R];


	float* jackoutputL    = buffers->audio[Buffers::JACK_TRACK_0_L + trackoffset];
	float* jackoutputR    = buffers->audio[Buffers::JACK_TRACK_0_R + trackoffset];

	for(unsigned int i = 0; i < nframes; i++) {

		//compute master volume lag;
		_toMasterLag += SMOOTHING_CONST * (_toMaster - _toMasterLag);

		// compute pan lag:
		_panLLag += SMOOTHING_CONST * (_panL - _panLLag);
		_panRLag += SMOOTHING_CONST * (_panR - _panRLag);

		// compute send volume lag:
		_toSendLag += SMOOTHING_CONST * (_toSend - _toSendLag);

		// compute sidechain signal lag
		_toPostSidechainLag += SMOOTHING_CONST * (_toPostSidechain - _toPostSidechainLag);
 
		// * master for "post-fader" sends
		float tmpL = trackBufferL[i];
		float tmpR = trackBufferR[i]; 

		// post-sidechain *moves* signal between "before/after" ducking, not add!
		masterL[i]       += tmpL * _toMasterLag * (1-_toPostSidechainLag) * _panLLag;
		masterR[i]       += tmpR * _toMasterLag * (1-_toPostSidechainLag) * _panRLag;
		if(jackoutputL)
			jackoutputL[i]     = tmpL * _toMasterLag * (1-_toPostSidechainLag);
		if(jackoutputR)
			jackoutputR[i]     = tmpR * _toMasterLag * (1-_toPostSidechainLag);
		if ( _toPostfaderActive ) {
			sendL[i]        += tmpL * _toSendLag * _toMasterLag;
			sendR[i]        += tmpR * _toSendLag * _toMasterLag;
		}

		if ( _toXSideActive ) {
			postSidechainL[i] += tmpL * _toPostSidechainLag * _toMasterLag;
			postSidechainR[i] += tmpR * _toPostSidechainLag * _toMasterLag;
		}

		// turning down an element in the mix should *NOT* influence sidechaining
		if ( _toKeyActive ) {
			sidechainL[i]     += tmpL;
			sidechainR[i]     += tmpR;
		}

	}
}

TrackOutput::~TrackOutput()
{
	delete dbMeter;
}
