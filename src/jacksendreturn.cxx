#include "jacksendreturn.hxx"
#include "jack.hxx"
#include <math.h>
#include <assert.h>
extern Jack* jack;
JackSendReturn::JackSendReturn(int trackid, AudioProcessor *prev, jack_client_t *client)
	:m_trackid(trackid), m_previousProcessor(prev), m_sendvol(1.0f)
{
	char name[50];
	sprintf(name, "Send_track_%d_l\n",trackid);
	m_sendport_l=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
	sprintf(name, "Send_track_%d_r\n",trackid);
	m_sendport_r=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
	sprintf(name, "Return_track_%d_l\n",trackid);
	m_returnport_l=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
	sprintf(name, "Return_track_%d_r\n",trackid);
	m_returnport_r=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
	m_active=false;
	m_counter=0;
}

void JackSendReturn::process(unsigned int nframes, Buffers *buffers)
{
	//Reset send buffer
	int offset=m_counter%(buffers->nframes);
	float* sendtrackL=&(buffers->audio[Buffers::SEND_TRACK_0_L+m_trackid][0]);
	float* sendtrackR=&(buffers->audio[Buffers::SEND_TRACK_0_R+m_trackid][0]);


	float* rettrackL=&(buffers->audio[Buffers::RETURN_TRACK_0_L+m_trackid][0]);
	float* rettrackR=&(buffers->audio[Buffers::RETURN_TRACK_0_R+m_trackid][0]);

	memset(sendtrackL,0,nframes*sizeof(float));
	memset(sendtrackR,0,nframes*sizeof(float));

	//Process previous AudioProcessor
	m_previousProcessor->process(nframes,buffers);
	float* sendL=(float*)jack_port_get_buffer(m_sendport_l,(jack_nframes_t)(buffers->nframes));
	float* sendR=(float*)jack_port_get_buffer(m_sendport_r,(jack_nframes_t)(buffers->nframes));
	float* retL=(float*)jack_port_get_buffer(m_returnport_l,(jack_nframes_t)(buffers->nframes));
	float* retR=(float*)jack_port_get_buffer(m_returnport_r,(jack_nframes_t)(buffers->nframes));
	if(offset) {
		sendL+=offset;
		sendR+=offset;
		retL+=offset;
		retR+=offset;
	}

	for(int i=0; i<nframes; i++) {
		sendL[i]=m_sendvol*sendtrackL[i];
		sendR[i]=m_sendvol*sendtrackR[i];
    }
//    if(nframes!=buffers->nframes)
//    {
//        cout<<send<<endl;
//    }

	if(offset)
		assert(offset+nframes==buffers->nframes);

	if(m_active) {
		memcpy(rettrackL,retL,nframes*sizeof(float));
		memcpy(rettrackR,retR,nframes*sizeof(float));
    }
	else {
		memcpy(rettrackL,
		       sendtrackL,nframes*sizeof(float));
		memcpy(rettrackR,
		       sendtrackR,nframes*sizeof(float));
    }
	m_counter+=nframes;

}

void JackSendReturn::activate(bool act)
{
	m_active=act;
}

void JackSendReturn::sendVolume(float vol)
{
	m_sendvol=vol;
}
