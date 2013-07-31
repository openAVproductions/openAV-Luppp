
#include "../jack.hxx"

#include <iostream>

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

void AkaiAPC::clipSelect(int t, int clip, ClipMode cm)
{
  unsigned char data[3];
  data[0] = 144 + t;
  data[1] = 53 + clip;
  
  switch (cm)
  {
    case CLIP_MODE_EMPTY:         data[2] = 0; break;
    case CLIP_MODE_PLAYING:       data[2] = 1; break;
    case CLIP_MODE_PLAY_QUEUED:   data[2] = 2; break;
    case CLIP_MODE_RECORDING:     data[2] = 3; break;
    case CLIP_MODE_RECORD_QUEUED: data[2] = 4; break;
    case CLIP_MODE_LOADED:        data[2] = 5; break;
    case CLIP_MODE_STOP_QUEUED:   data[2] = 6; break;
  }
  
  jack->writeApcOutput( &data[0] );
}

void AkaiAPC::mute(int t, bool b)
{
  
}


void AkaiAPC::volume(int t, float f)
{
  
}

void note_on( int track, int note, int vel )
{
  printf("apc note on using MIDI observer \n");
}

void cc_change( int track, int cc, float value )
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
    note_on( b1 - 144, b2, b3 );
  }
  else if ( b1 >= 128 && b1 < 128 + 16 ) // NOTE_OFF
  {
    //note_off( b1 - 144, b2, b3 );
  }
  else if ( b1 >= 176 && b1 < 176 + 16 ) // CC
  {
    cc_change( b1 - 176, b2, data[2] / 127.f );
  }
}
