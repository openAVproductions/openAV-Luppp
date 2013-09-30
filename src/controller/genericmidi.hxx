

#ifndef LUPPP_GENERIC_MIDI_H
#define LUPPP_GENERIC_MIDI_H

#include "controller.hxx"

#include <vector>

#include "../observer/midi.hxx"

/// for future compatibility, LupppAction might be a string mapped to a unique number
typedef std::string LupppAction;

class Binding
{
  public:
    Binding(unsigned char b1, unsigned char b2, LupppAction act) : status(b1), data(b2), action(act) {}
    
    unsigned char status;
    unsigned char data;
    
    /// the action this binding relates to
    LupppAction action;
    
    /// arguments to the event: track number, scene number etc
    int track;
    int scene;
};

/** GenericMIDI
 *  This class is used to load a <controller>.cfg JSON file as a MIDI map.
 *  The name parameter is the name of the JACK MIDI ports that are registered.
**/
class GenericMIDI : public Controller, public MidiObserver
{
  public:
    /// Loads 
    GenericMIDI(std::string file, std::string name);
    
    std::string getName();
    
    /// track actions
    //void mute(int t, bool b);
    
    void launchScene( int scene );
    
    void volume(int t, float f);
    /*
    void progress(int t, int s, float f);
    void recordArm(int t, bool b);
    void launchScene( int scene );
    void setSceneState(int track, int clip, GridLogic::State s);
    
    /// track FX
    void trackSend(int t, int send, float v);
    */
    
    void reset();
    
    void midi(unsigned char* data);
    
  private:
    int _port;
    
    std::string name;
    
    /// contains midi binding instances
    std::vector<Binding> midiToAction;
    std::vector<Binding> actionToMidi;
    
    int loadController(std::string controllerFile);
    
    
    
    /// for "sampling" a clip in the grid, and applying events to it:
    /// footpedal for example
    bool shiftPressed;
    int footpedalTrack;
    int footpedalScene;
    
    /*
    /// for handling events
    void ccChange( int track, int cc, float value );
    void noteOff( int track, int note, int vel );
    void noteOn( int track, int note, int vel );
    */
};

#endif // LUPPP_GENERIC_MIDI_H

