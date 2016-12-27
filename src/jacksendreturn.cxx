#include "jacksendreturn.hxx"
#include "jack.hxx"
#include <math.h>
#include <assert.h>
extern Jack* jack;
JackSendReturn::JackSendReturn(int trackid, AudioProcessor *prev, jack_client_t *client)
	:m_trackid(trackid), m_previousProcessor(prev), m_sendvol(1.0f)
{
	char name[50];
	sprintf(name, "Send_track_%d\n",trackid);
	m_sendport=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
	sprintf(name, "Return_track_%d\n",trackid);
	m_returnport=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
	m_active=false;
	m_counter=0;
}

void JackSendReturn::process(unsigned int nframes, Buffers *buffers)
{
	//Reset send buffer
	int offset=m_counter%(buffers->nframes);
	float* sendtrack=&(buffers->audio[Buffers::SEND_TRACK_0+m_trackid][0]);


	float* rettrack=&(buffers->audio[Buffers::RETURN_TRACK_0+m_trackid][0]);

	memset(sendtrack,0,nframes*sizeof(float));

	//Process previous AudioProcessor
	m_previousProcessor->process(nframes,buffers);
	float* send=(float*)jack_port_get_buffer(m_sendport,(jack_nframes_t)(buffers->nframes));
	float* ret=(float*)jack_port_get_buffer(m_returnport,(jack_nframes_t)(buffers->nframes));
	if(offset) {
		send+=offset;
		ret+=offset;
	}

	for(int i=0; i<nframes; i++)
		send[i]=m_sendvol*sendtrack[i];
//    if(nframes!=buffers->nframes)
//    {
//        cout<<send<<endl;
//    }

	if(offset)
		assert(offset+nframes==buffers->nframes);

	if(m_active)
		memcpy(rettrack,ret,nframes*sizeof(float));
	else
		memcpy(rettrack,
		       sendtrack,nframes*sizeof(float));
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
