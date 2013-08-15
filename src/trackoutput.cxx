
#include "trackoutput.hxx"

// valgrind no access code
//#include <valgrind/memcheck.h>
//VALGRIND_MAKE_MEM_NOACCESS( &_trackBuffer[0] , MAX_BUFFER_SIZE );

TrackOutput::TrackOutput(int t, AudioProcessor* ap) :
  AudioProcessor(),
  track(t),
  previousInChain(ap),
  dbMeter(44100)
{
  //printf("trackOutput ID: %i, ap = ", track );
  //std::cout << ap << std::endl;
  
  // UI update
  uiUpdateConstant = 44100 / 30;
  uiUpdateCounter  = 44100 / 30;
  
  _toReverb        = 0.0;
  _toMaster        = 0.8;
  _toSidechain     = 0.0;
  _toPostSidechain = 0.0;
}


void TrackOutput::setMaster(float value)
{
  _toMaster = value;
}

float TrackOutput::getMaster()
{
  return _toMaster;
}


void TrackOutput::setSend( int send, float value )
{
  switch( send )
  {
    case SEND_REV:
        _toReverb = value;
        break;
    case SEND_SIDE:
        _toSidechain = value;
        break;
    case SEND_POST:
        _toPostSidechain = value;
        break;
  }
}

void TrackOutput::process(unsigned int nframes, Buffers* buffers)
{
  // get & zero track buffer
  float* trackBuffer = buffers->audio[Buffers::TRACK_0 + track];
  memset( trackBuffer, 0, nframes );
  
  // call process() up the chain
  previousInChain->process( nframes, buffers );
  
  
  // run the meter
  dbMeter.process( nframes, trackBuffer, trackBuffer );
  
  if (uiUpdateCounter > uiUpdateConstant )
  {
    // FIXME: should be using ControllerUpdater
    EventTrackSignalLevel e( track, dbMeter.getLeftDB() * _toMaster, dbMeter.getRightDB() * _toMaster );
    writeToGuiRingbuffer( &e );
    uiUpdateCounter = 0;
  }
  
  uiUpdateCounter += nframes;
  
  // copy audio data into reverb / sidechain / master buffers
  float* reverb        = buffers->audio[Buffers::REVERB];
  float* sidechain     = buffers->audio[Buffers::SIDECHAIN];
  float* postSidechain = buffers->audio[Buffers::POST_SIDECHAIN];
  
  float* masterL       = buffers->audio[Buffers::MASTER_OUT_L];
  float* masterR       = buffers->audio[Buffers::MASTER_OUT_R];
  
  for(unsigned int i = 0; i < nframes; i++)
  {
    float tmp = trackBuffer[i];
    
    masterL[i]       += tmp * _toMaster;
    masterR[i]       += tmp * _toMaster;
    
    reverb[i]        += tmp * _toReverb;
    sidechain[i]     += tmp * _toSidechain;
    postSidechain[i] += tmp * _toPostSidechain;
  }
}

TrackOutput::~TrackOutput()
{
}
