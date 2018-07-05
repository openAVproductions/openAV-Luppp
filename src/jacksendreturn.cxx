#include "jacksendreturn.hxx"
#include "jack.hxx"
#include <math.h>
#include <assert.h>
extern Jack* jack;
JackSendReturn::JackSendReturn(int trackid, AudioProcessor *prev, jack_client_t *client)
	:_trackId(trackid), _previousProcessor(prev), _sendVol(1.0f)
{
	char name[50];
	int trackid_human = trackid + 1;
	sprintf(name, "Send_track_%d_l\n",trackid_human);
	_sendPortL=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
	sprintf(name, "Send_track_%d_r\n",trackid_human);
	_sendPortR=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
	sprintf(name, "Return_track_%d_l\n",trackid_human);
	_returnPortL=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
	sprintf(name, "Return_track_%d_r\n",trackid_human);
	_returnPortR=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
	_active=false;
	_counter=0;
}

void JackSendReturn::process(unsigned int nframes, Buffers *buffers)
{
	// index = first-track + (track * channels)
	int trackoffset = _trackId * NCHANNELS;
	//Reset send buffer
	int offset=_counter%(buffers->nframes);
	float* sendtrackL=&(buffers->audio[Buffers::SEND_TRACK_0_L + trackoffset][0]);
	float* sendtrackR=&(buffers->audio[Buffers::SEND_TRACK_0_R + trackoffset][0]);


	float* rettrackL=&(buffers->audio[Buffers::RETURN_TRACK_0_L + trackoffset][0]);
	float* rettrackR=&(buffers->audio[Buffers::RETURN_TRACK_0_R + trackoffset][0]);

	memset(sendtrackL,0,nframes*sizeof(float));
	memset(sendtrackR,0,nframes*sizeof(float));

	//Process previous AudioProcessor
	_previousProcessor->process(nframes,buffers);

	float* sendL=(float*)jack_port_get_buffer(_sendPortL,  (jack_nframes_t)(buffers->nframes));
	float* sendR=(float*)jack_port_get_buffer(_sendPortR,  (jack_nframes_t)(buffers->nframes));
	float* retL =(float*)jack_port_get_buffer(_returnPortL,(jack_nframes_t)(buffers->nframes));
	float* retR =(float*)jack_port_get_buffer(_returnPortR,(jack_nframes_t)(buffers->nframes));

	if(offset) {
		sendL+=offset;
		sendR+=offset;
		retL+=offset;
		retR+=offset;
	}

	for(int i=0; i<nframes; i++) {
		_sendVolLag += SMOOTHING_CONST * (_sendVol - _sendVolLag);

		sendL[i] = _sendVolLag * sendtrackL[i];
		sendR[i] = _sendVolLag * sendtrackR[i];
	}

	if(offset)
		assert(offset+nframes==buffers->nframes);

	if(_active) {
		memcpy(rettrackL,retL,nframes*sizeof(float));
		memcpy(rettrackR,retR,nframes*sizeof(float));
	}
	else {
		memcpy(rettrackL, sendtrackL,nframes*sizeof(float));
		memcpy(rettrackR, sendtrackR,nframes*sizeof(float));
	}
	_counter+=nframes;

}

void JackSendReturn::activate(bool act)
{
	_active=act;
}

void JackSendReturn::sendVolume(float vol)
{
	_sendVol=vol;
}
