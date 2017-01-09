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
	_toMasterDiff    = 0;
	_toMasterPan     = 0.f;
	_toReverb        = 0.0;
	_toSidechain     = 0.0;
	_toPostSidechain = 0.0;

	_toPostfaderActive        = 0;
	_toKeyActive     = 0;
	_toXSideActive = true;
}


void TrackOutput::setMaster(float value)
{
	if(value < 0.01)
		value = 0.f;
	_toMaster = value;
	_toMasterDiff=_toMaster-_toMasterLag;
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
	_toMasterPan = pan;
}

void TrackOutput::setSend( int send, float value )
{
	switch( send ) {
	case SEND_POSTFADER:
		_toReverb = value;
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
	//compute master volume lag;
	if(fabs(_toMaster-_toMasterLag)>=fabs(_toMasterDiff/100.0))
		_toMasterLag+=_toMasterDiff/10.0;
	// get & zero track buffer
	float* trackBuffer = buffers->audio[Buffers::RETURN_TRACK_0 + track];
	memset( trackBuffer, 0, sizeof(float)*nframes );

	// call process() up the chain
	previousInChain->process( nframes, buffers );

	// run the meter
	dbMeter->process( nframes, trackBuffer, trackBuffer );

	if (uiUpdateCounter > uiUpdateConstant ) {
		float l = dbMeter->getLeftDB() * _toMasterLag;
		float r = dbMeter->getRightDB() * _toMasterLag;
		EventTrackSignalLevel e( track, l, r );
		writeToGuiRingbuffer( &e );
		uiUpdateCounter = 0;
	}

	uiUpdateCounter += nframes;

	// copy audio data into reverb / sidechain / master buffers
	float* reverb        = buffers->audio[Buffers::SEND];
	float* sidechain     = buffers->audio[Buffers::SIDECHAIN_KEY];
	float* postSidechain = buffers->audio[Buffers::SIDECHAIN_SIGNAL];

	float* masterL       = buffers->audio[Buffers::MASTER_OUT_L];
	float* masterR       = buffers->audio[Buffers::MASTER_OUT_R];


	float* jackoutput    = buffers->audio[Buffers::JACK_TRACK_0+track];

	/* Trial + Error leads to this algo - its cheap and cheerful */
	float p = ((_toMasterPan + 1.0f) * 0.5f) * (M_PI * 0.5);
	float sin_p = sin(p);
	float cos_p = cos(p);
	float pan_r = sin_p * sin_p;
	float pan_l = cos_p * cos_p;

	for(unsigned int i = 0; i < nframes; i++) {
		// * master for "post-fader" sends
		float tmp = trackBuffer[i];

		// post-sidechain *moves* signal between "before/after" ducking, not add!
		masterL[i]       += tmp * _toMasterLag * (1-_toPostSidechain) * pan_l;
		masterR[i]       += tmp * _toMasterLag * (1-_toPostSidechain) * pan_r;
		if(jackoutput)
			jackoutput[i]     = tmp * _toMasterLag * (1-_toPostSidechain);
		if ( _toPostfaderActive )
			reverb[i]        += tmp * _toReverb * _toMasterLag;

		if ( _toXSideActive )
			postSidechain[i] += tmp * _toPostSidechain * _toMasterLag;

		// turning down an element in the mix should *NOT* influence sidechaining
		if ( _toKeyActive )
			sidechain[i]     += tmp;

	}
}

TrackOutput::~TrackOutput()
{
	delete dbMeter;
}
