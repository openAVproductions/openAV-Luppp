

#include "genericmidi.hxx"

#include <iostream>

#include "../jack.hxx"
#include "../gridlogic.hxx"

extern Jack* jack;

GenericMIDI::GenericMIDI() :
  Controller(),
  MidiObserver("generic")
{
  _port = port();
}

void GenericMIDI::volume(int t, float f)
{
  cout << "t " << t << "  vol " << f << endl;
}


/*




void GenericMIDI::recordArm(int t, bool enabled)
{
  unsigned char data[3];
  data[0] = 144 + t;
  data[1] = 48; // record enable LED
  data[2] = enabled ? 127 : 0;
  jack->midiObserverWriteMIDI( _port,  &data[0] );
}

void GenericMIDI::progress(int t, int s, float f)
{
  /*
  unsigned char data[3];
  data[0] = 176;
  data[1] = 48; // record enable LED
  data[2] = 127 * f;
  jack->midiObserverWriteMIDI( _port,  &data[0] );
  *
}

void GenericMIDI::trackSend(int t, int send, float v)
{
  if ( t >= NTRACKS)
  {
    // master track
    return;
  }
  
  unsigned char data[3];
  
  if ( send == SEND_SIDE )
  {
    data[0] = 144 + t;
    data[1] = 49;
    data[2] = v > 0.5 ? 127 : 0 ;
  }
  else if ( send == SEND_POST )
  {
    data[0] = 176 + t;
    data[1] = 16;
    data[2] = 127 * v;
  }
  else if ( send == SEND_REV )
  {
    data[0] = 176 + t;
    data[1] = 17;
    data[2] = 127 * v;
  }
  
  jack->midiObserverWriteMIDI( _port,  &data[0] );
}

void GenericMIDI::setSceneState(int t, int clip, GridLogic::State s)
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
    case GridLogic::STATE_STOPPED:       data[2] = 5; break;
    case GridLogic::STATE_STOP_QUEUED:   data[2] = 6; break;
  }
  
  jack->midiObserverWriteMIDI( _port,  &data[0] );
}

void GenericMIDI::launchScene( int s )
{
  unsigned char data[3];
  
  for(int i = 0; i < 5; i++ )
  {
    data[0] = 128;
    data[1] = 82 + i; // scene play
    data[2] = 0;
    jack->midiObserverWriteMIDI( _port,  &data[0] );
  }
  
  data[0] = 144;
  data[1] = 82 + s;
  data[2] = 127;
  jack->midiObserverWriteMIDI( _port,  &data[0] );
}

void GenericMIDI::mute(int t, bool b)
{
  
}


void GenericMIDI::volume(int t, float f)
{
  
}

void GenericMIDI::noteOn( int track, int note, int vel )
{
  printf("apc noteOn: t = %i, n = %i, vel = %i\n", track, note, vel);
  if ( note >= 53 && note <= 57 )
  {
    if ( shiftPressed )
    {
      footpedalTrack = track;
      footpedalScene = note - 53;
    }
    else
      jack->getGridLogic()->pressed( track, note - 53 );
    return;
  }
  
  
  switch( note )
  {
    case 48: { // record
        jack->getLogic()->trackRecordArm(track, true);
        } break;
    case 49: { // solo / cue
        jack->getLogic()->trackSend(track, SEND_SIDE, 1);
        } break;
    
    case 82: // Master Scene Clips
    case 83: 
    case 84: 
    case 85: 
    case 86: {
        jack->getGridLogic()->launchScene(note - 82);
        } break;
    
    case 98:  { // Shift
        shiftPressed = true;
        break; }
    
    case 99: { // tap tempo
        jack->getLogic()->tapTempo();
        jack->getControllerUpdater()->tapTempo( true );
        } break; 
    case 100: { // nudge +
        
        } break; 
    case 101: { // nudge -
        
        } break; 
    
    default:
        break;
  }
  
  
}

void GenericMIDI::noteOff( int track, int note, int vel )
{
  printf("apc noteOff: t = %i, n = %i, vel = %i\n", track, note, vel);
  if ( note >= 53 && note <= 57 )
  {
    jack->getGridLogic()->released( track, note - 53 );
  }
  
  switch( note )
  {
    case 48: { // record
        jack->getLogic()->trackRecordArm(track, false);
        } break;
    case 49: { // solo / cue
        jack->getLogic()->trackSend(track, SEND_SIDE, 0);
        }
    
    case 99: { // tap tempo
        EventTimeTempoTap e(false);
        writeToGuiRingbuffer( &e );
        } break; 
    
    case 82: // scene launch
    case 83:
    case 84:
    case 85:
    case 86: {
        int s = jack->getGridLogic()->getLaunchedScene();
        launchScene( s );
        } break ;
    
    case 98:  { // Shift
        shiftPressed = false;
        break; }
  }
}

void GenericMIDI::ccChange( int track, int cc, float value )
{
  if ( track >= 0 && track < NTRACKS )
  {
    switch( cc )
    {
      /// Track faders
      case 7: {
          jack->getLogic()->trackVolume( track, value );
          break; }
      case 14: { // master
          jack->getLogic()->trackVolume( -1, value );
          break; }
      
      /// X-Fader
      case 15: {
          jack->getTimeManager()->setBpm( 60 + value * 180 );
          break; }
      
      /// Device Control
      case 16: {
          //jack->getLogic()->trackSend( track, SEND_SIDE, value );
          break; }
      case 17: {
          jack->getLogic()->trackSend( track, SEND_POST, value );
          break; }
      case 18: {
          jack->getLogic()->trackSend( track, SEND_REV, value );
          break; }
      
      
      case 64: { // FootSwitch 1
          if ( value > 0.5 )
          {
            printf("footpedal press %i, %i\n", footpedalTrack, footpedalScene );
            jack->getGridLogic()->pressed( footpedalTrack, footpedalScene );
          }
          else
          {
            printf("footpedal release %i, %i\n", footpedalTrack, footpedalScene );
            jack->getGridLogic()->released( footpedalTrack, footpedalScene );
          } break; }
      case 67:  { // FootSwitch 2
          
          break; }
    }
  }
  else
  // tracks outside normal track range
  // handle master track buttons etc here
  {
    if ( cc == 16 ) // master device control select
    {
      
    }
  }
}

*/


void GenericMIDI::midi(unsigned char* data)
{
  int b1 = data[0];
  int b2 = data[1];
  int b3 = data[2];
  
  if ( b1 >= 144 && b1 < 144 + 16 ) // NOTE_ON
  {
    //noteOn( b1 - 144, b2, b3 );
  }
  else if ( b1 >= 128 && b1 < 128 + 16 ) // NOTE_OFF
  {
    //noteOff( b1 - 128, b2, b3 );
  }
  else if ( b1 >= 176 && b1 < 176 + 16 ) // CC
  {
    //ccChange( b1 - 176, b2, data[2] / 127.f );
  }
}


void GenericMIDI::reset()
{
  /*
  unsigned char data[3];
  // setup "volume" style rotary display
  for(int i = 0; i < NTRACKS; i++)
  {
    data[0] = 176 + i;
    data[1] = 0x19;
    data[2] = 2;
    jack->midiObserverWriteMIDI( _port,  &data[0] );
  }
  */
}
