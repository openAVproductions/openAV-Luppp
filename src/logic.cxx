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

#include "logic.hxx"

#include "audioengine.hxx"
extern AudioEngine* g_pAudioEngine;

#include "jack.hxx"
extern Jack* jack;

#include "timemanager.hxx"
#include "controllerupdater.hxx"
#include "trackoutput.hxx"
#include "metronome.hxx"
#include "jacksendreturn.hxx"

Logic::Logic()
{

}

void Logic::tapTempo()
{
	g_pAudioEngine->getTimeManager()->tap();
}

void Logic::setBpm(float bpm)
{
	g_pAudioEngine->getTimeManager()->queueBpmChange( bpm );
}

void Logic::setBpmZeroOne(float bpm)
{
	g_pAudioEngine->getTimeManager()->queueBpmChangeZeroOne( bpm );
}

void Logic::metronomeEnable(bool b)
{
	g_pAudioEngine->getMetronome()->setActive(b);
	g_pAudioEngine->getControllerUpdater()->metronomeEnable( b );
}

void Logic::masterInputVol( float v )
{
	g_pAudioEngine->inputVolume( v );
	g_pAudioEngine->getControllerUpdater()->masterInputVol( v );
}

void Logic::masterInputTo( int to, float v )
{
	g_pAudioEngine->inputTo( (Event::INPUT_TO)to, v );
	g_pAudioEngine->getControllerUpdater()->masterInputTo( to, v );
}

void Logic::masterInputToActive( int inputTo, bool active)
{
	g_pAudioEngine->inputToActive( (Event::INPUT_TO)inputTo, active);
	g_pAudioEngine->getControllerUpdater()->masterInputToActive( (int)inputTo, active );
}

void Logic::masterReturn( int returnNum, float value )
{
	g_pAudioEngine->returnVolume( value );
	g_pAudioEngine->getControllerUpdater()->masterReturnVolume( value );
}

void Logic::trackPan(int t, float p)
{
	if ( t >= 0 && t < NTRACKS ) {
		g_pAudioEngine->getTrackOutput( t )->setPan( p );
		g_pAudioEngine->getControllerUpdater()->pan( t, p );
	}
}

void Logic::trackVolume(int t, float v)
{
	if ( t == -1 ) { // master track
		g_pAudioEngine->masterVolume(v);
		g_pAudioEngine->getControllerUpdater()->masterVolume( v );
	} else if ( t >= 0 && t < NTRACKS ) {
		g_pAudioEngine->getTrackOutput( t )->setMaster( v );
		g_pAudioEngine->getControllerUpdater()->volume( t, v );
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::trackRecordArm(int t, bool v)
{
	if ( t >= 0 && t < NTRACKS ) {
		jack->getTrackOutput( t )->recordArm( v );
		jack->getControllerUpdater()->recordArm( t, v );
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::trackSendActive(int t, int s, bool v)
{
	if ( t >= 0 && t < NTRACKS ) {
		jack->getTrackOutput( t )->setSendActive( s, v );
		jack->getControllerUpdater()->setTrackSendActive( t, s, v );
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::trackSend(int t, int send, float v)
{
	if ( t >= 0 && t < NTRACKS ) {
		jack->getTrackOutput( t )->setSend( send, v );
		jack->getControllerUpdater()->setTrackSend( t, send, v );
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::trackJackSendActivate(int t, bool active)
{
	if ( t >= 0 && t < NTRACKS ) {
#ifdef AUDIO_BACKEND_JACK
		jack->getJackSendReturn(t)->activate(active);
		jack->getControllerUpdater()->setTrackJackSendActive( t, active );
#else
		LUPPP_NOTE("trackJackSendActivate not available with RtAudio backend");
#endif
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::trackJackSend(int t, float vol)
{
	if ( t >= 0 && t < NTRACKS ) {
#ifdef AUDIO_BACKEND_JACK
		jack->getJackSendReturn(t)->sendVolume(vol);
		jack->getControllerUpdater()->setTrackJackSend( t, vol );
#else
		LUPPP_NOTE("trackJackSend not available with RtAudio backend");
#endif
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}

void Logic::looperClipLenght(int t, int s, int l)
{
	if ( t >= 0 && t < NTRACKS ) {
		jack->getLooper( t )->getClip( s )->setBeats(l);
	} else {
		LUPPP_WARN("invalid track number %i: check controller map has \"track\" field.", t );
	}
}


void Logic::looperUseAsTempo(int t, int s)
{
	size_t clipBeats  = jack->getLooper( t )->getClip( s )->getBeats();
	size_t clipFrames = jack->getLooper( t )->getClip( s )->getBufferLenght();

	if ( clipBeats > 0 ) {
		size_t framesPerBeat = clipFrames / clipBeats;
		jack->getTimeManager()->queueFpbChange( framesPerBeat );
	}

}
