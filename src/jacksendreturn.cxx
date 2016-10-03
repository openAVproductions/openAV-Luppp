#include "jacksendreturn.hxx"
#include "jack.hxx"
extern Jack* jack;
JackSendReturn::JackSendReturn(int trackid, AudioProcessor *prev, jack_client_t *client)
    :m_trackid(trackid), m_previousProcessor(prev)
{
    char name[50];
    sprintf(name, "Send_track_%d\0",trackid);
    m_sendport=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsOutput,0);
    sprintf(name, "Return_track_%d\0",trackid);
    m_returnport=jack_port_register(client,name,JACK_DEFAULT_AUDIO_TYPE,JackPortIsInput,0);
}

void JackSendReturn::process(unsigned int nframes, Buffers *buffers)
{
    //Reset send buffer
    memset(buffers->audio[Buffers::SEND_TRACK_0+m_trackid],0,nframes*sizeof(float));

    //Process previous AudioProcessor
    m_previousProcessor->process(nframes,buffers);
    float* send=(float*)jack_port_get_buffer(m_sendport,(jack_nframes_t)nframes);
    float* ret=(float*)jack_port_get_buffer(m_returnport,(jack_nframes_t)nframes);

    //Copy result of previous AudioProcessor to send port
    memcpy(send,buffers->audio[Buffers::SEND_TRACK_0+m_trackid],nframes*sizeof(float));

    //Get connection status
    bool connected=jack_port_connected(m_returnport);
    //Is return port connected?
    //Yes then grab the audio data from the connected port
    //No: get the audio from the previous AudioProcessor
    if(connected)
        memcpy(buffers->audio[Buffers::RETURN_TRACK_0+m_trackid],ret,nframes*sizeof(float));
    else
        memcpy(buffers->audio[Buffers::RETURN_TRACK_0+m_trackid],
                buffers->audio[Buffers::SEND_TRACK_0+m_trackid],nframes*sizeof(float));

}
