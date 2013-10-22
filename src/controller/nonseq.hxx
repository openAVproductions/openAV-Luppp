

#ifndef LUPPP_NON_SEQ_H
#define LUPPP_NON_SEQ_H

#include "../observer/midi.hxx"
#include "controller.hxx"

/** NonSeq
 *  A class for integrating the NonSeq sequencer with Luppp, providing scene
 *  on / off MIDI events to NonSeq
**/
class NonSeq : public Controller, public MidiIO
{
  public:
    NonSeq();
    ~NonSeq(){};
    
    std::string getName();
    
    int registerComponents();
    
    void launchScene( int scene );
    
    void setSceneState(int track, int scene, GridLogic::State s);
};

#endif // LUPPP_NON_SEQ_H
