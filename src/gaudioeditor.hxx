

#ifndef LUPPP_AUDIO_EDITOR_H
#define LUPPP_AUDIO_EDITOR_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class AudioBuffer;
namespace Avtk {
class Waveform;
class Button;
}

class AudioEditor
{
  public:
    AudioEditor();
    ~AudioEditor();
    
    /// shows the window, and loads the audio buffer into the display
    void show( AudioBuffer* ab, bool modal = false );
    
    /// returns true if the editor window is shown
    bool shown();
    
    void setBeatsAndQuit( int beats );
    
    //AudioBuffer* getAudioBuffer();
  
  private:
    // GUI elements
    Fl_Double_Window* window;
    Avtk::Waveform*   waveform;
    Avtk::Button*     cancel;
    Avtk::Button*     beatButtons[7];
    
    // Contents
    AudioBuffer* ab;
};

#endif // LUPPP_AUDIO_EDITOR_H

