
#ifndef LUPPP_AUDIO_PROCESSOR_H
#define LUPPP_AUDIO_PROCESSOR_H

class AudioProcessor
{
  public:
    AudioProcessor(){}
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    virtual void process(int nframes, Buffers* buffers)
    {
      
      
    }
    
    virtual ~AudioProcessor(){};
  
  private:
    int track;
    
    float _toMaster;
    
    float _toReverb;
    float _toSidechain;
    float _toPostSidechain;
  
};

#endif // LUPPP_AUDIO_PROCESSOR_H

