

#include "gui.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../gridlogic.hxx"

extern Jack* jack;

LupppGUI::LupppGUI() :
  Controller()
{
  
}

void LupppGUI::recordArm(int t, bool enabled)
{
  
}

void LupppGUI::setSceneState(int t, int clip, GridLogic::State s)
{
  
}

void LupppGUI::mute(int t, bool b)
{
  
}


void LupppGUI::volume(int t, float f)
{
  
}

void LupppGUI::progress(int t, int s, float f)
{
  EventLooperProgress e( t, f );
  writeToGuiRingbuffer( &e );
}
