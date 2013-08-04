

#include "apc.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../gridlogic.hxx"

extern Jack* jack;

AkaiAPC::AkaiAPC() :
  Controller(),
  MidiObserver("apc")
{
  
}

void AkaiAPC::recordArm(int t, bool enabled)
{
  unsigned char data[3];
  data[0] = 144 + t;
  data[1] = 48; // record enable LED
  data[2] = enabled ? 127 : 0;
  jack->writeApcOutput( &data[0] );
}

void AkaiAPC::progress(int t, int s, float f)
{
  unsigned char data[3];
  data[0] = 176;
  data[1] = 48; // record enable LED
  data[2] = 127 * f;
  jack->writeApcOutput( &data[0] );
}

void AkaiAPC::setSceneState(int t, int clip, GridLogic::State s)
{
  unsigned char data[3];
  data[0] = 144 + t;
  data[1] = 53 + clip;
  
  switch (s)
  {
    case GridLogic::STATE_EMPTY:         data[2] = 0; break;
    case GridLogic::STATE_PLAYING:       data[2] = 1; break;
    case GridLogic::STATE_PLAY_QUEUED:   data[2] = 2; break;
    case GridLogic::STATE_RECORDING:     data[2] = 3; break;
    case GridLogic::STATE_RECORD_QUEUED: data[2] = 4; break;
    case GridLogic::STATE_LOADED:        data[2] = 5; break;
    case GridLogic::STATE_STOP_QUEUED:   data[2] = 6; break;
  }
  
  jack->writeApcOutput( &data[0] );
}

void AkaiAPC::mute(int t, bool b)
{
  
}


void AkaiAPC::volume(int t, float f)
{
  
}

void noteOn( int track, int note, int vel )
{
  printf("apc noteOn: t = %i, n = %i, vel = %i\n", track, note, vel);
  if ( note >= 53 && note <= 57 )
  {
    jack->getGridLogic()->pressed( track, note - 53 );
  }
}

void noteOff( int track, int note, int vel )
{
  printf("apc noteOff: t = %i, n = %i, vel = %i\n", track, note, vel);
  if ( note >= 53 && note <= 57 )
  {
    jack->getGridLogic()->released( track, note - 53 );
  }
}

void ccChange( int track, int cc, float value )
{
  switch( cc )
  {
    case 7: {
        EventTrackVol e( track, value );
        writeToGuiRingbuffer( &e ); }
        break;
  }
}

void AkaiAPC::midi(unsigned char* data)
{
  int b1 = data[0];
  int b2 = data[1];
  int b3 = data[2];
  
  if ( b1 >= 144 && b1 < 144 + 16 ) // NOTE_ON
  {
    noteOn( b1 - 144, b2, b3 );
  }
  else if ( b1 >= 128 && b1 < 128 + 16 ) // NOTE_OFF
  {
    noteOff( b1 - 128, b2, b3 );
  }
  else if ( b1 >= 176 && b1 < 176 + 16 ) // CC
  {
    ccChange( b1 - 176, b2, data[2] / 127.f );
  }
}
