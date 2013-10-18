

#include "genericmidi.hxx"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "../jack.hxx"
#include "../logic.hxx"
#include "../gridlogic.hxx"

#include "../eventhandler.hxx"

extern Jack* jack;

GenericMIDI::GenericMIDI() :
  Controller(),
  MidiIO()
{
  name = "generic";
  registerMidiPorts( name );
  stat = CONTROLLER_OK;
}

GenericMIDI::GenericMIDI(std::string file) :
  Controller(),
  MidiIO()
{
  // default name for each controller
  name = "generic";
  
  // load the JSON config file
  int result = loadController( file );
  
  if ( result == LUPPP_RETURN_OK )
  {
    registerMidiPorts( name );
    stat = CONTROLLER_OK;
  }
  else
  {
    LUPPP_ERROR("Error in loading controller map!" );
    stat = CONTROLLER_ERROR;
  }
}

const std::vector<Binding*>& GenericMIDI::getMidiToAction()
{
  return midiToAction;
}

Controller::STATUS GenericMIDI::status()
{
  return stat;
}


int GenericMIDI::registerComponents()
{
  // makes JACK add this controller to the midiObservers list:
  // note the static_cast<>() is *needed* here for multiple-inheritance
  MidiIO* m = static_cast<MidiIO*>(this);
  
  jack->registerMidiIO( m );
  
  return LUPPP_RETURN_OK;
}

std::string GenericMIDI::getName()
{
  return name;
}

void GenericMIDI::volume(int t, float f)
{
  //cout << "t " << t << "  vol " << f << endl;
}


void GenericMIDI::recordArm(int t, bool enabled)
{
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == TRACK_RECORD_ARM && b->track == t )
    {
      unsigned char data[3];
      data[0] = b->status;
      data[1] = b->data;
      data[2] = enabled ? 127 : 0;
      writeMidi( data );
      return;
    }
  }
}

void GenericMIDI::metronomeEnable(bool enabled)
{
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == METRONOME_ACTIVE )
    {
      unsigned char data[3];
      data[0] = b->status;
      data[1] = b->data;
      data[2] = enabled ? 127 : 0;
      writeMidi( data );
      return;
    }
  }
}

void GenericMIDI::trackSend(int t, int send, float v)
{
  
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == TRACK_SEND && b->track == t && b->send == send )
    {
      unsigned char data[3];
      data[0] = b->status;
      data[1] = b->data;
      data[2] = v * 127;
      writeMidi( data );
      return;
    }
  }
}

void GenericMIDI::trackSendActive(int t, int send, bool a)
{
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == TRACK_SEND_ACTIVE && b->track == t && b->send == send )
    {
      unsigned char data[3];
      data[0] = b->status;
      data[1] = b->data;
      data[2] = a ? 127 : 0;
      writeMidi( data );
      return;
    }
  }
}

/*
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
  
  if ( send == SEND_KEY )
  {
    data[0] = 144 + t;
    data[1] = 49;
    data[2] = v > 0.5 ? 127 : 0 ;
  }
  else if ( send == SEND_XSIDE )
  {
    data[0] = 176 + t;
    data[1] = 16;
    data[2] = 127 * v;
  }
  else if ( send == SEND_POSTFADER )
  {
    data[0] = 176 + t;
    data[1] = 17;
    data[2] = 127 * v;
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
        jack->getLogic()->trackSend(track, SEND_KEY, 1);
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
        jack->getLogic()->trackSend(track, SEND_KEY, 0);
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
          //jack->getLogic()->trackSend( track, SEND_KEY, value );
          break; }
      case 17: {
          jack->getLogic()->trackSend( track, SEND_XSIDE, value );
          break; }
      case 18: {
          jack->getLogic()->trackSend( track, SEND_POSTFADER, value );
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
  
  //LUPPP_NOTE("GenericMIDI::midi() %i %i %f", status, data, value );
  
  // create new MIDI binding?
  if ( jack->bindingEventRecordEnable )
  {
    setupBinding( jack->bindingEventType, status, data,
                  jack->bindingTrack,
                  jack->bindingScene,
                  jack->bindingSend,
                  jack->bindingActive );
    
    // binding is now created, so disable GUI binding enable button
    jack->bindingEventRecordEnable = false;
    EventControllerBindingEnable e( false );
    writeToGuiRingbuffer( &e );
    
    // update GUI of new binding
    EventControllerBindingMade e2( (void*)midiToAction.back() );
    writeToGuiRingbuffer( &e2 );
  }
  
  // iterate over bindings, execute binding action if matches
  for(unsigned int i = 0; i < midiToAction.size(); i++)
  {
    Binding* b = midiToAction.at(i);
    
    if ( b->status == status && b->data == data )
    {
      //LUPPP_NOTE("Executing action %i, value %f, b->active %i", b->action, value, int(b->active) );
      
      switch( b->action )
      {
        case Event::TRACK_VOLUME: jack->getLogic()->trackVolume( b->track, value ); break;
        case Event::TRACK_SEND:   jack->getLogic()->trackSend( b->track, b->send, value ); break;
        case Event::TRACK_SEND_ACTIVE: jack->getLogic()->trackSendActive( b->track, b->send, b->active ); break;
        case Event::TRACK_RECORD_ARM: jack->getLogic()->trackRecordArm( b->track, b->active ); break;
        
        case Event::GRID_EVENT:
            if ( b->active )
              jack->getGridLogic()->pressed( b->track, b->scene );
            else
              jack->getGridLogic()->released( b->track, b->scene );
            break;
        
        case Event::GRID_SELECT_CLIP_EVENT:
            jack->getGridLogic()->selectedTrackSceneEvent( value );
            break;
        case Event::GRID_SELECT_CLIP_ENABLE:
            jack->getGridLogic()->setSelectTrackScene( b->active );
            break;
        case Event::GRID_LAUNCH_SCENE:
            jack->getGridLogic()->launchScene( b->scene );
            break;
        
        
        case Event::METRONOME_ACTIVE:
            jack->getLogic()->metronomeEnable( b->active );
            break;
        
        case Event::MASTER_VOL:   jack->getLogic()->trackVolume( -1     , value ); break;
      }
    }
    
    
  }
  
}

void GenericMIDI::setSceneState(int t, int scene, GridLogic::State s)
{
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == GRID_STATE && b->track == t && b->scene == scene )
    {
      for( map<int,int>::iterator it = b->clipStateMap.begin(); it != b->clipStateMap.end(); ++it)
      {
        // check if its the right clip state
        if ( it->first == int(s) )
        {
          unsigned char data[3];
          data[0] = b->status;
          data[1] = b->data;
          data[2] = it->second;
          
          //LUPPP_NOTE("GenericMIDI::sceneState() writing event %i, %i, %i", data[0],data[1],data[2] );
          writeMidi( data );
        }
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
  for(unsigned int i = 0; i < actionToMidi.size(); i++)
  {
    Binding* b = actionToMidi.at(i);
    
    if ( b->action == GRID_LAUNCH_SCENE )
    {
      for( int i = 0; i < 5; i++ )
      {
        if ( i != scene )
        {
          unsigned char data[3];
          data[0] = b->status;
          data[1] = b->data + i;
          data[2] = 0;
          writeMidi( data );
        }
      }
      unsigned char data[3];
      data[0] = b->status;
      data[1] = b->data + scene;
      data[2] = 127;
      //LUPPP_NOTE("this = %i GenericMIDI::launchScene()", this );
      writeMidi( data );
      
      return;
    }
  }
  
}


int GenericMIDI::loadController( std::string file )
{
  
  /// open and read whole file
#ifdef DEBUG_CONTROLLER
  LUPPP_NOTE("%s%s","Loading controller : ", file.c_str() );
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
      std::string error = cJSON_GetErrorPtr();
      
      std::istringstream ss( error );
      
      std::string line;
      std::getline( ss, line );
      LUPPP_ERROR("%s %s","Error in JSON *before*: ", line.c_str() );
      
      for(int i = 0; i < 5; i++)
      {
        std::getline( ss, line );
        LUPPP_ERROR("%s %s","Error in JSON         : ", line.c_str() );
      }
      return LUPPP_RETURN_ERROR;
    }
    
    cJSON* nameJson = cJSON_GetObjectItem( controllerJson, "name" );
    if ( nameJson )
    {
      name = nameJson->valuestring;
      LUPPP_NOTE("Device %s", name.c_str() );
    }
    else
    {
        LUPPP_NOTE("Has no name field");
    }
    
    
    
    cJSON* inputBindings = cJSON_GetObjectItem( controllerJson, "inputBindings");
    if ( inputBindings )
    {
      int nBindings = cJSON_GetArraySize( inputBindings );
      for(int i = 0; i < nBindings; i++ )
      {
        cJSON* bindingJson = cJSON_GetArrayItem( inputBindings, i );
        Binding* tmp = setupBinding( bindingJson );
        if ( tmp )
          midiToAction.push_back( tmp );
      }
    }
    else
    {
      LUPPP_WARN("%s %s","No controller bindings found at ", file.c_str() );
      return LUPPP_RETURN_WARNING;
    }
    
    
    
    cJSON* outputBindings = cJSON_GetObjectItem( controllerJson, "outputBindings");
    if ( outputBindings )
    {
      int nBindings = cJSON_GetArraySize( outputBindings );
      for(int i = 0; i < nBindings; i++ )
      {
        cJSON* bindingJson = cJSON_GetArrayItem( outputBindings, i );
        Binding* tmp = setupBinding( bindingJson );
        if ( tmp )
          actionToMidi.push_back( tmp );
        
        //LUPPP_NOTE("Binding from %s to %i %i", actionJ->valuestring, statusJson->valueint, dataJson->valueint );
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
  
  
  LUPPP_NOTE("Controller loading complete." );
  
  return LUPPP_RETURN_OK;
}

void GenericMIDI::setupBinding( LupppAction eventType, int midiStatus, int midiData, int track, int scene, int send, int active )
{
  LUPPP_NOTE("MIDI binding, track %d, from eventType %d to %d, %d", track, eventType, midiStatus, midiData );
  
  // FIXME: NON-RT Have stack of Bindings() available, or push in GUI thread?
  Binding* tmp = new Binding();
  
  tmp->action = eventType;
  tmp->status = midiStatus;
  tmp->data   = midiData;
  tmp->track  = track;
  tmp->scene  = scene;
  tmp->send   = send;
  tmp->active = active;
  
  // FIXME: Could allocate memory! Issue? Shouldn't be binding during performance?
  midiToAction.push_back( tmp );
}


Binding* GenericMIDI::setupBinding( cJSON* binding )
{
  // create binding, then fill in data as from JSON.
  Binding* tmp = new Binding();
  
  cJSON* statusJson = cJSON_GetObjectItem( binding, "status" );
  cJSON* dataJson   = cJSON_GetObjectItem( binding, "data"   );
  if ( !statusJson || !dataJson )
  {
    LUPPP_WARN("Binding doesn't have status / data field: fix .ctlr file");
    return 0;
  }
  
  cJSON* actionJson = cJSON_GetObjectItem( binding, "action" );
  if ( !actionJson )
  {
    LUPPP_WARN("Binding doesn't have action field: fix .ctlr file");
    return 0;
  }
  
  tmp->status = statusJson->valueint;
  tmp->data   = dataJson->valueint;
  
  if ( strcmp( actionJson->valuestring, "track:volume" ) == 0 ) {
    tmp->action = Event::TRACK_VOLUME;
  } else if ( strcmp( actionJson->valuestring, "track:send" ) == 0 ) {
    tmp->action = Event::TRACK_SEND;
    tmp->send = Event::SEND_POSTFADER;
  }
  else if ( strcmp( actionJson->valuestring, "track:xside" ) == 0 ) {
    tmp->action = Event::TRACK_SEND;
    tmp->send = Event::SEND_XSIDE;
  }
  else if ( strcmp( actionJson->valuestring, "track:sendactive" ) == 0 ) {
    tmp->action = Event::TRACK_SEND_ACTIVE;
    tmp->send = Event::SEND_POSTFADER;
  }
  else if ( strcmp( actionJson->valuestring, "track:keyactive" ) == 0 ) {
    tmp->action = Event::TRACK_SEND_ACTIVE;
    tmp->send = Event::SEND_KEY;
  }
  else if ( strcmp( actionJson->valuestring, "track:recordarm" ) == 0 ) {
    tmp->action = Event::TRACK_RECORD_ARM;
  }
  
  else if ( strcmp( actionJson->valuestring, "track:clippressed" ) == 0 ) {
    tmp->action = Event::GRID_EVENT;
    tmp->active = 1; // press event
  }
  else if ( strcmp( actionJson->valuestring, "track:clipreleased" ) == 0 ) {
    tmp->action = Event::GRID_EVENT;
    tmp->active = 0; // release event
  }
  else if ( strcmp( actionJson->valuestring, "track:clipstate" ) == 0 ) {
    tmp->action = Event::GRID_STATE;
    
    // read "state", to bind multiple values depending on clip state
    
    cJSON* stateBindings = cJSON_GetObjectItem( binding, "state");
    if ( stateBindings )
    {
      cJSON* empty      = cJSON_GetObjectItem( stateBindings, "empty");
      cJSON* stopped    = cJSON_GetObjectItem( stateBindings, "stopped");
      cJSON* playing    = cJSON_GetObjectItem( stateBindings, "playing");
      cJSON* recording  = cJSON_GetObjectItem( stateBindings, "recording");
      cJSON* qPlaying   = cJSON_GetObjectItem( stateBindings, "queuePlaying");
      cJSON* qStopped   = cJSON_GetObjectItem( stateBindings, "queueStopped");
      cJSON* qRecording = cJSON_GetObjectItem( stateBindings, "queueRecording");
      
      // keep a map of GridLogic::STATE to the MIDI byte 3 output from .ctlr file
      if ( empty )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_EMPTY, empty->valueint ) );
      if ( stopped )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_STOPPED, stopped->valueint ) );
      if ( playing )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_PLAYING, playing->valueint ) );
      if ( recording )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_RECORDING, recording->valueint ) );
      
      if ( qPlaying )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_PLAY_QUEUED, qPlaying->valueint ) );
      if ( qStopped )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_STOP_QUEUED, qStopped->valueint ) );
      if ( qRecording )
        tmp->clipStateMap.insert( std::pair<int,int>( GridLogic::STATE_RECORD_QUEUED, qRecording->valueint ) );
    }
    /* // Extreme debugging
    for( map<int,int>::iterator it = tmp->clipStateMap.begin(); it != tmp->clipStateMap.end(); ++it)
    {
      std::cout << it->first << " " << it->second << "\n";
    }
    */
  }
  
  else if ( strcmp( actionJson->valuestring, "gridlogic:launchscene" ) == 0 ) {
    tmp->action = Event::GRID_LAUNCH_SCENE;
  }
  else if ( strcmp( actionJson->valuestring, "gridlogic:selectclipevent" ) == 0 ) {
    tmp->action = Event::GRID_SELECT_CLIP_EVENT;
  }
  else if ( strcmp( actionJson->valuestring, "gridlogic:selectclipenable" ) == 0 ) {
    tmp->action = Event::GRID_SELECT_CLIP_ENABLE;
  }
  else if ( strcmp( actionJson->valuestring, "master:volume" ) == 0 ) {
    tmp->action = Event::MASTER_VOL;
  }
  else if ( strcmp( actionJson->valuestring, "metronome:active" ) == 0 ) {
    tmp->action = Event::METRONOME_ACTIVE;
  }
  
  // check for valid event: otherwise pass
  if ( tmp->action != Event::EVENT_NULL )
  {
    //LUPPP_NOTE("Binding from %i %i  %s", statusJson->valueint, dataJson->valueint, actionJson->valuestring);
    
    cJSON* track      = cJSON_GetObjectItem( binding, "track"  );
    cJSON* scene      = cJSON_GetObjectItem( binding, "scene"  );
    cJSON* activeJson = cJSON_GetObjectItem( binding, "active" );
    
    if ( track )
      tmp->track = track->valueint;
    if ( scene )
      tmp->scene = scene->valueint;
    if ( activeJson )
      tmp->active = activeJson->valueint;
    
    return tmp;
  }
  else
  {
    LUPPP_WARN("Binding action not recognized: %s", actionJson->valuestring );
  }
  
  return 0;
  
}



