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


#include "guicontroller.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../event.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"

extern Jack* jack;

LupppGUI::LupppGUI() :
	Controller()
{
}

void LupppGUI::masterInputVol(float f)
{
	EventMasterInputVol e( f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::masterInputTo(int to,float f)
{
	EventMasterInputTo e( (Event::INPUT_TO)to, f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::masterInputToActive(int to,float f)
{
	EventMasterInputToActive e( (Event::INPUT_TO)to, f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::masterVolume(float f)
{
	EventMasterVol e( f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::masterReturnVolume(float f)
{
	printf(" return %f ", f );
	EventMasterReturn e( RETURN_MAIN, f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::metronomeEnable(bool r)
{
	EventMetronomeActive e( r );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::recordArm(int t, bool r)
{
	EventTrackRecordArm e( t, r );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::trackSend(int t, int send, float r)
{
	EventTrackSend e( t, static_cast<Event::SEND_TYPE>(send), r );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::specialScene(int t, int s)
{
	EventGridSelectNewChosen e( t, s );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::trackSendActive(int t, int send, bool a)
{
	EventTrackSendActive e( t, static_cast<Event::SEND_TYPE>(send), a );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::trackJackSend(int t, float v)
{
	EventTrackJackSend e(t,v);
	writeToGuiRingbuffer(&e);
}

void LupppGUI::trackJackSendActivate(int t, bool a)
{
	EventTrackJackSendActivate e(t,a);
	writeToGuiRingbuffer(&e);
}

void LupppGUI::setSceneState(int t, int clip, GridLogic::State s)
{
	EventGridState e( t, clip, s );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::launchScene( int scene )
{
	EventGridLaunchScene e( scene );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::mute(int t, bool b)
{

}

void LupppGUI::tapTempo( bool b )
{
	EventTimeTempoTap e(b);
	writeToGuiRingbuffer( &e );
}

void LupppGUI::transportState(enum TRANSPORT_STATE ts)
{
	EventTransportState e(ts);
	writeToGuiRingbuffer( &e );
}

void LupppGUI::bpm(int bpm)
{
	EventTimeBPM e(bpm);
	writeToGuiRingbuffer( &e );
}

void LupppGUI::volume(int t, float f)
{
	EventTrackVol e( t, f );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::pan(int t, float p)
{
	EventTrackPan e( t, p );
	writeToGuiRingbuffer( &e );
}

void LupppGUI::progress(int t, int s, float f)
{
	EventLooperProgress e( t, f );
	writeToGuiRingbuffer( &e );
}
