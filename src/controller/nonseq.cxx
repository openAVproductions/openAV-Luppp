

#include "nonseq.hxx"

#include <string>

#include "../jack.hxx"
#include "../looper.hxx"
#include "../looperclip.hxx"

NonSeq::NonSeq() :
  Controller(),
  MidiIO()
{
  std::string name = "non-seq";
  registerMidiPorts( name );
}

void NonSeq::launchScene( int scene )
{
  unsigned char data[3];
  data[0] = 176;
  data[1] = 20;
  data[2] = scene;
  
  //LUPPP_NOTE("NonSeq::launchScene() %i, %i, %i\n", data[0],data[1],data[2] );
  writeMidi( data );
}

std::string NonSeq::getName()
{
  return "non-seq";
}


void NonSeq::setSceneState(int track, int scene, GridLogic::State s)
{
  unsigned char data[3];
  data[0] = 176;
  data[1] = 22; // off
  
  // check *actual* value of playing: *NOT* GridState::s, because it could be queued
  // for something else, but we want the *actual* here, not "queued". This is a unique
  // use case because were trying to control non-seq as if it were part of Luppp.
  if( jack->getLooper( track )->getClip( scene )->playing() )
    data[1] = 21;
  
  data[2] = track;
  
  //LUPPP_NOTE("NonSeq::setSceneState() %i, %i, %i\n", data[0],data[1],data[2] );
  writeMidi( data );
}


int NonSeq::registerComponents()
{
  // makes JACK add this controller to the midiObservers list:
  // note the static_cast<>() is *needed* here for multiple-inheritance
  MidiIO* m = static_cast<MidiIO*>(this);
  
  jack->registerMidiIO( m );
  
  return LUPPP_RETURN_OK;
}
