

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
  printf("special cene writing event\n");
  EventGridSelectNewChosen e( t, s );
  writeToGuiRingbuffer( &e );
}

void LupppGUI::trackSendActive(int t, int send, bool a)
{
  EventTrackSendActive e( t, static_cast<Event::SEND_TYPE>(send), a );
  writeToGuiRingbuffer( &e );
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

void LupppGUI::progress(int t, int s, float f)
{
  EventLooperProgress e( t, f );
  writeToGuiRingbuffer( &e );
}
