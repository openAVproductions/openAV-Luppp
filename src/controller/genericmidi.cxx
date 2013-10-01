

#include "genericmidi.hxx"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "../jack.hxx"
#include "../gridlogic.hxx"
#include "../cjson/cJSON.h"

extern Jack* jack;

GenericMIDI::GenericMIDI(std::string file, std::string name) :
  Controller(),
  MidiObserver()
{
  // load the JSON config file
  loadController( file );
  
  
  
  // register the JACK MIDI ports
  //registerPorts( name );
  // FIXME: this port is the input port, gotta fix the output port ID number
  //_port = port();
}


void GenericMIDI::process(int nframes)
{
  // get port buffers and setup
  void* inputBuffer = (void*) jack_port_get_buffer( (jack_port_t*)jackInputPort, nframes );
  void* outputBuffer= (void*) jack_port_get_buffer( (jack_port_t*)jackInputPort, nframes );
  jack_midi_clear_buffer( outputBuffer );
  
  jack_midi_event_t in_event;
  int index = 0;
  int event_count = (int) jack_midi_get_event_count( inputBuffer );
  while ( index < event_count )
  {
    jack_midi_event_get(&in_event, inputBuffer, index);
    midi( (unsigned char*) &in_event.buffer[0] );
    //printf( "%s MIDI %i %i %i\n", midiObservers.at(i)->getName().c_str(), int(in_event.buffer[0]), int(in_event.buffer[1]), int(in_event.buffer[2]) );
    index++;
  }
  
}

int GenericMIDI::registerComponents()
{

}

std::string GenericMIDI::getName()
{
  return name;
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
  
  unsigned char data[3];
  data[0] = 176;
  data[1] = 48; // record enable LED
  data[2] = 127 * f;
  jack->midiObserverWriteMIDI( _port,  &data[0] );
  
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


void GenericMIDI::midi(unsigned char* midi)
{
  int status = midi[0];
  int data   = midi[1];
  float value  = midi[2] / 127.f;
  
  //LUPPP_NOTE("called %i %i %f", status, data, value );
  
  // iterate over bindings, execute binding action if matches
  for(unsigned int i = 0; i < midiToAction.size(); i++)
  {
    if ( midiToAction.at(i).status == status &&
         midiToAction.at(i).data   == data )
    {
      Binding& b = midiToAction.at(i);
      //LUPPP_NOTE("Executing action %s", b.action.c_str() );
      
      if( b.action.compare("track:volume") == 0 ) {
        jack->getLogic()->trackVolume( b.track, value );
      }
      if( b.action.compare("footpedal") == 0 ) {
        LUPPP_NOTE("Executing action %s v = %f", b.action.c_str(), value );
        //jack->getLogic()->trackVolume( b.track, value );
      }
      
      
      
    }
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

void GenericMIDI::launchScene( int scene )
{
  unsigned char data[3];
  data[0] = 144;
  data[1] = 82 + scene;
  data[2] = 127;
  //jack->midiObserverWriteMIDI( _port,  &data[0] );
}


int GenericMIDI::loadController( std::string file )
{
  LUPPP_NOTE("%s%s","Loading controller : ", file.c_str() );
  
  /// open and read whole file
#ifdef DEBUG_CONTROLLER
  cout << "loading controller file " << file.c_str() << endl;
#endif
  std::ifstream sampleFile( file.c_str(), std::ios_base::in|std::ios_base::ate);
  long file_length = sampleFile.tellg();
  if ( file_length > 0 )
  {
    sampleFile.seekg(0, std::ios_base::beg);
    sampleFile.clear();
    char *sampleString = new char[file_length];
    sampleFile.read(sampleString, file_length);
    
    cJSON* controllerJson = cJSON_Parse( sampleString );
    if (!controllerJson) {
      LUPPP_ERROR("%s %s","Error in Sample JSON before: ", cJSON_GetErrorPtr() );
      return LUPPP_RETURN_ERROR;
    }
    
    cJSON* name = cJSON_GetObjectItem( controllerJson, "name" );
    if ( name )
    {
      LUPPP_NOTE("Device %s", name->valuestring );
    }
    
    
    
    cJSON* inputBindings = cJSON_GetObjectItem( controllerJson, "inputBindings");
    if ( inputBindings )
    {
      int nBindings = cJSON_GetArraySize( inputBindings );
      for(int i = 0; i < nBindings; i++ )
      {
        cJSON* binding = cJSON_GetArrayItem( inputBindings, i );
        
        // collect essential data
        cJSON* status = cJSON_GetObjectItem( binding, "status" );
        cJSON* data   = cJSON_GetObjectItem( binding, "data"   );
        cJSON* action = cJSON_GetObjectItem( binding, "action" );
        
        // collect event metadata
        cJSON* track  = cJSON_GetObjectItem( binding, "track"  );
        
        LUPPP_NOTE("Binding from %i %i  %s", status->valueint, data->valueint, action->valuestring);
        
        midiToAction.push_back( Binding(status->valueint, data->valueint, action->valuestring ) );
        
        if ( track )
          midiToAction.back().track = track->valueint;
        
      }
    }
    else
    {
      LUPPP_WARN("%s %s","No controller bindings found at ", file.c_str() );
      return LUPPP_RETURN_WARNING;
    }
    
    
    
    cJSON* feedbackBindings = cJSON_GetObjectItem( controllerJson, "feedbackBindings");
    if ( feedbackBindings )
    {
      int nBindings = cJSON_GetArraySize( feedbackBindings );
      for(int i = 0; i < nBindings; i++ )
      {
        cJSON* binding = cJSON_GetArrayItem( feedbackBindings, i );
        
        cJSON* action = cJSON_GetObjectItem( binding, "action" );
        
        cJSON* status = cJSON_GetObjectItem( binding, "status" );
        cJSON* data   = cJSON_GetObjectItem( binding, "data"   );
        
        LUPPP_NOTE("Binding from %s to %i %i", action->valuestring, status->valueint, data->valueint );
        
        actionToMidi.push_back( Binding(status->valueint, data->valueint, action->valuestring ) );
      }
    }
    else
    {
      LUPPP_WARN("%s %s","No controller bindings found at ", file.c_str() );
      return LUPPP_RETURN_WARNING;
    }
    
    
    
    cJSON_Delete( controllerJson );
    delete[] sampleString;
  }
  else
  {
    LUPPP_WARN("%s %s","No controller file found at ", file.c_str() );
    return LUPPP_RETURN_WARNING;
  }
  
  
  return LUPPP_RETURN_OK;
}
