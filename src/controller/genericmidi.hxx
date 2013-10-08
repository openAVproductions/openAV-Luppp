

#ifndef LUPPP_GENERIC_MIDI_H
#define LUPPP_GENERIC_MIDI_H

#include "controller.hxx"

#include <map>
#include <string>
#include <vector>

#include "../cjson/cJSON.h"
#include "../observer/midi.hxx"

/// for future compatibility, LupppAction might be a string mapped to a unique number
typedef int LupppAction;

class Binding
{
  public:
    Binding() : status(0), data(0), action(0), active(false),track(-2),scene(-1),send(-1){}
    
    unsigned char status;
    unsigned char data;
    
    /// the action this binding relates to: this is an integer based on the
    /// event.hxx enumeration of event types
    LupppAction action;
    
    /// arguments to the event: track number, scene number etc
    int active;
    int track;
    int scene;
    int send;
    
    std::map<int,int> clipStateMap;
};

/** GenericMIDI
 *  This class is used to load a <controller>.cfg JSON file as a MIDI map.
 *  The name parameter is the name of the JACK MIDI ports that are registered.
**/
class GenericMIDI : public Controller, public MidiIO
{
  public:
    /// Loads 
    GenericMIDI(std::string file);
    
    int registerComponents();
    
    Controller::STATUS status();
    
    std::string getName();
    
    /// track actions
    //void mute(int t, bool b);
    
    void metronomeEnable(bool b);
    
    void launchScene( int scene );
    
    void volume(int t, float f);
    
    
    void recordArm(int t, bool b);
    void setSceneState(int track, int clip, GridLogic::State s);
    
    /*
    void progress(int t, int s, float f);
    void launchScene( int scene );
    
    /// track FX
    void trackSend(int t, int send, float v);
    */
    
    void trackSend(int t, int send, float v);
    void trackSendActive(int t, int send, bool a);
    
    
    void reset();
    
    void midi(unsigned char* data);
    
    void process(int nframes);
    
  private:
    STATUS stat;
    
    std::string name;
    
    /// contains midi binding instances
    std::vector<Binding*> midiToAction;
    std::vector<Binding*> actionToMidi;
    
    int loadController(std::string controllerFile);
    
    /// creates a binding from a cJSON inputBindings / outputBindings object
    Binding* setupBinding( cJSON* bindings );
    
    
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

