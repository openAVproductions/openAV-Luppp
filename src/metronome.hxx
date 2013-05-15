
#ifndef LUPPP_METRONOME_H
#define LUPPP_METRONOME_H

#include <cmath>
#include <iostream>

#include "buffers.hxx"

#include "observer/observer.hxx"

using namespace std;

// simple metronome class
class Metronome : public Observer
{
  public:
    Metronome() :
      playPoint  (0),
      playBar    (false)
    {
      // create beat and bar samples
      endPoint = (44100.f/441);
      // samples per cycle of 
      float scale = 2 * 3.1415 / endPoint;
      
      /*And fill it up*/
      for(int i=0;i < endPoint*40;i++){
        beatSample[i]= sin(i*scale);
        barSample [i]= sin(i*scale*1.5);
      }
    }
    
    void bar()
    {
      playPoint = 0;
      playBar = true;
    }
    
    void beat()
    {
      playPoint = 0;
      //cout << "Looper " << track << " got beat()" << flush;
    }
    
    void setFpb(int f)
    {
      fpb = f;
      //cout << "Looper " << track << " got fpb of " << fpb << endl;
    }
    
    void process(int nframes, Buffers* buffers)
    {
      float* out = buffers->audio[Buffers::MASTER_OUTPUT];
      
      float* sample = &beatSample[0];
      if( playBar ) { sample = &barSample[0]; playBar = false; }
      
      for(int i = 0; i < nframes; i++)
      {
        if ( playPoint < endPoint )
        {
          out[i] += sample[playPoint++];
        }
        
      }
    }
  
  private:
    int fpb;
    bool playBar;
    
    int playPoint, endPoint;
    float barSample[44100];
    float beatSample[44100];
  
};

#endif // LUPPP_METRONOME_H
