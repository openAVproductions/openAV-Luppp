

#include "gaudioeditor.hxx"

#include "config.hxx"

#include "audiobuffer.hxx"

#include "avtk/waveform.h"
#include "avtk/avtk_button.h"


AudioEditor::AudioEditor()
{
  window = new Fl_Double_Window(500,230,"Audio Editor");
  waveform = new Avtk::Waveform(5, 5, 450, 150, "Waveform");
  window->end();
}

void AudioEditor::show( AudioBuffer* buf, bool modal )
{
  
  
  ab = buf;
  
  if ( !ab )
  {
    LUPPP_WARN("called with ab == 0");
  }
  else
  {
    std::vector<float>& tmp = ab->getData();
    waveform->setData( &tmp[0], tmp.size() );
  }
  
  
  window->set_modal();
  
  window->show();
}

bool AudioEditor::shown()
{
  return window->shown();
}

AudioEditor::~AudioEditor()
{
  delete window;
}
