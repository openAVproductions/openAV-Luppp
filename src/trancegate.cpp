
#include "trancegate.hpp"

#include "top.hpp"

#include <iostream>

using namespace std;

TranceGate::TranceGate(Top* t, EffectType et) : Effect(t,et)
{
  top = t;
  ID = Effect::getID();
  
  std::cout << "TranceGate() ID = " << ID << std::endl;
  
  active = false;
  
  gateAttack = 12000;
  
  oldBeat = -1;
  
}

void TranceGate::process(int nframes, float *buffer)
{
  //std::cout << "Process: queueActive: " << queueActive << "\tActive: " << active << std::endl;
  
  int framesPerBeat = (int) top->samplerate / (top->bpm / 60.0);
  int newBeat = top->frameNumber / framesPerBeat;
  
  if ( oldBeat != newBeat )
  {
    // new beat, retrigger the AHDSR
    gate = 12000;
    oldBeat = newBeat;
  }
  
  for( int i = 0; i < nframes; i++)
  {
    if ( gate < 0 )
    {
      *buffer = 0.f; // null samples if gate is off
    }
    
    buffer++;
    gate--;
  }
}
