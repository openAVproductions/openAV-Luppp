
#include "../jack.hxx"

#include <iostream>

extern Jack* jack;

AkaiAPC::AkaiAPC()
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
    //case CLIP_MODE_RECORDING:     data[2] = 4; break;
    case CLIP_MODE_LOADED:        data[2] = 5; break;
  }
  
  jack->writeApcOutput( &data[0] );
}

void AkaiAPC::mute(int t, bool b)
{
  
}


void AkaiAPC::volume(int t, float f)
{
  
}
