
#ifndef LUPPP_TRACK_OUTPUT_H
#define LUPPP_TRACK_OUTPUT_H

class TrackOutput
{
  public:
    TrackOutput(int t, AudioProcessor* ap) :
      track(t),
      previousInChain(ap),
    {
    }
    
    /// copies the track output to master buffer, sidechain & post-side buffer
    void process(int nframes, Buffers* buffers)
    {
      ap->process( nframes, buffers );
      
      for(int i = 0; i < nframes; i++)
      {
        // copy data here
      }
    }
  
  private:
    int track;
    
    float _toMaster;
    
    float _toReverb;
    float _toSidechain;
    float _toPostSidechain;
    
    /// Pointer to "previous" processor: the graph is backwards
    AudioProcessor* previousInChain;
  
};

#endif // LUPPP_TRACK_OUTPUT_H

