
#include "event.hxx"

// defines the pretty names of each event type
const char* EventMasterVol::prettyName = "Master Volume";
const char* EventMasterReturn::prettyName = "Master Return";
const char* EventMasterInputVol::prettyName = "Input Volume";
const char* EventMasterInputTo::prettyName = "Input To";
const char* EventMasterInputToActive::prettyName = "Input To Active";

const char* EventTrackVol::prettyName = "Track Volume";
const char* EventTrackSend::prettyName = "Track Send";
const char* EventTrackSendActive::prettyName = "Track Send Active";
const char* EventTrackRecordArm::prettyName = "Track Record Arm";

const char* EventTimeBPM::prettyName = "Tempo BPM";
const char* EventTimeTempoTap::prettyName = "Tempo Tap";

const char* EventGridEvent::prettyName = "Grid Press";
const char* EventGridLaunchScene::prettyName = "Launch Scene";



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
