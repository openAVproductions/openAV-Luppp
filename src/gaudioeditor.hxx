

#ifndef LUPPP_AUDIO_EDITOR_H
#define LUPPP_AUDIO_EDITOR_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

//#include "avtk/waveform.h"

class AudioBuffer;

class AudioEditor
{
  public:
    AudioEditor();
    ~AudioEditor();
    
    /// shows the window, and loads the audio buffer into the display
    void show( AudioBuffer* ab );
  
  private:
    // GUI elements
    Fl_Double_Window* window;
    //Avtk::Waveform*   waveform;
    
    // Contents
    AudioBuffer* ab;
};

#endif // LUPPP_AUDIO_EDITOR_H

