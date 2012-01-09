
#include "beatsmash.hpp"

#include "top.hpp"

BeatSmash::BeatSmash(Top* t, EffectType et) : Effect(t,et)
{
  top = t;
  ID = Effect::getID();
  
  std::cout << "BeatSmash() ID = " << ID << std::endl;
  
  active = false;
  queueActive = false;
  
  audioBuffer.resize(22050, 0.f);
}

void BeatSmash::process(int nframes, float *L)
{
  //std::cout << "Process: queueActive: " << queueActive << "\tActive: " << active << std::endl;
  //active = top->state.beatSmash;
  
  for( int i = 0; i < nframes; i++)
  {
    // always push back new
    audioBuffer.push_back( L[i] );
    
    // if needed playback delays... :-)
    if (active)
    {
      // audioBuffer contains the last beat of samples
      //
      // OLDEST                                       Newest
      //   |                                            |
      //   +============================================+
      //   |                      |                     |
      //   0         audioBuffer.size() / 2             audioBuffer.size()
      
      // iter over number of taps
      L[i] += audioBuffer.at( 9000 * (float(active)/7.f) );
    }
    
    // always pop front
    audioBuffer.pop_front();
  }
}
