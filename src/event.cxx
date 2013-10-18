
#include "event.hxx"

// defines the pretty names of each event type
const char* EventMasterVol::prettyName = "Master Volume";
const char* EventMasterReturn::prettyName = "Master Return";
const char* EventMasterInputVol::prettyName = "Master Input Volume";
const char* EventMasterInputTo::prettyName = "Master Input To";
const char* EventMasterInputToActive::prettyName = "Master Input To Active";

const char* EventTrackVol::prettyName = "Track Volume";
const char* EventTrackSend::prettyName = "Track Send";
const char* EventTrackSendActive::prettyName = "Track Send Active";
const char* EventTrackRecordArm::prettyName = "Track Record Arm";

const char* EventTimeBPM::prettyName = "Tempo BPM";
const char* EventTimeTempoTap::prettyName = "Tempo Tap";

const char* EventGridEvent::prettyName = "Grid Press / Release";
const char* EventGridLaunchScene::prettyName = "Grid Launch Scene";
