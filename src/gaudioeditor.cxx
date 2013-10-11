

#include "gaudioeditor.hxx"

#include "config.hxx"

#include "audiobuffer.hxx"

#include "avtk/avtk_button.h"

AudioEditor::AudioEditor()
{
  window = new Fl_Double_Window(500,230,"Audio Editor");
  Avtk::Button* audioWaveform = new Avtk::Button(5, 5, 450, 220, "test");
  window->end();
}

void AudioEditor::show( AudioBuffer* buf )
{
  ab = buf;
  
  if ( !ab )
  {
    LUPPP_WARN("called with ab == 0");
    //return;
  }
  
  window->show();
}


AudioEditor::~AudioEditor()
{
  delete window;
}
