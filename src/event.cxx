
#include "event.hxx"

// defines the pretty names of each event type
const char* EventMasterVol::prettyName = "master:volume";
const char* EventMasterReturn::prettyName = "master:return";
const char* EventMasterInputVol::prettyName = "master:input_vol";
const char* EventMasterInputTo::prettyName = "master:input_to";
const char* EventMasterInputToActive::prettyName = "master:input_to_active";

const char* EventTrackVol::prettyName = "track:volume";
const char* EventTrackSend::prettyName = "track:send";
const char* EventTrackSendActive::prettyName = "track:send_active";
const char* EventTrackRecordArm::prettyName = "track:record_arm";

const char* EventTimeBPM::prettyName = "tempo_bpm";
const char* EventTimeTempoTap::prettyName = "tempo_tap";
const char* EventMetronomeActive::prettyName = "metronome:active";

const char* EventGridEvent::prettyName = "grid:event";
const char* EventGridLaunchScene::prettyName = "grid:launch_scene";

EVENT_TYPE Event::getTypeFromName(const char* name)
{
  for(int i = 0; i < EVENT_TYPE_FINAL; i++)
  {
    const char* tmp = getPrettyName(i);
    if ( tmp )
    {
      if ( strcmp( name, tmp ) == 0 ) {
        return (EVENT_TYPE)i;
      }
    }
  }
}

const char* Event::getPrettyName( int type )
{
  switch ( type )
  {
    case MASTER_VOL:{ return EventMasterVol::prettyName; }
    case MASTER_RETURN:{ return EventMasterReturn::prettyName; }
    case MASTER_INPUT_VOL:{ return EventMasterInputVol::prettyName; }
    case MASTER_INPUT_TO:{ return EventMasterInputTo::prettyName; }
    case MASTER_INPUT_TO_ACTIVE:{ return EventMasterInputToActive::prettyName; }
    
    case TRACK_VOLUME:{ return EventTrackVol::prettyName; }
    case TRACK_SEND:{ return EventTrackSend::prettyName; }
    case TRACK_SEND_ACTIVE:{ return EventTrackSendActive::prettyName; }
    case TRACK_RECORD_ARM:{ return EventTrackRecordArm::prettyName; }
    
    case TIME_BPM:{ return EventTimeBPM::prettyName; }
    case TIME_TEMPO_TAP:{ return EventTimeTempoTap::prettyName; }
    
    case GRID_EVENT:{ return EventGridEvent::prettyName; }
    case GRID_LAUNCH_SCENE:{ return EventGridLaunchScene::prettyName; }
    
    default: return 0;
  }
}
