

#include "gaudioeditor.hxx"

#include <sstream>

#include "config.hxx"

#include "audiobuffer.hxx"

#include "avtk/waveform.h"
#include "avtk/avtk_button.h"


void oneCB      (Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 1 ); }
void twoCB      (Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 2 ); }
void fourCB     (Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 4 ); }
void eightCB    (Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 8 ); }
void sixteenCB  (Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 16); }
void thirtyTwoCB(Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 32); }
void sixtyfourCB(Fl_Widget*,void* data) { ((AudioEditor*)data)->setBeatsAndQuit( 64); }

void cancelCB(Fl_Widget*,void* data)
{
  printf("button, beats = 4\n");
  AudioEditor* ae = (AudioEditor*) data;
  ae->setBeatsAndQuit( -1 );
}

AudioEditor::AudioEditor()
{
  window = new Fl_Double_Window(460,200,"Audio Editor : Beats?");
  waveform = new Avtk::Waveform(5, 5, 450, 150, "Waveform");
  cancel = new Avtk::Button(360, 160, 80,30, "Cancel");
  
  const char* names[] = {
    "1","2","4","8","16","32","64"
  };
  
  for(int i = 0; i < 7; i++)
  {
    stringstream s;
    s << i;
    beatButtons[i] = new Avtk::Button(5 + i * 50, 160, 40,30, strdup(names[i]) );
  }
  window->end();
  
  beatButtons[0]->callback( oneCB      , this);
  beatButtons[1]->callback( twoCB      , this);
  beatButtons[2]->callback( fourCB     , this);
  beatButtons[3]->callback( eightCB    , this);
  beatButtons[4]->callback( sixteenCB  , this);
  beatButtons[5]->callback( thirtyTwoCB, this);
  beatButtons[6]->callback( sixtyfourCB, this);
  
  cancel->callback( cancelCB, this );
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

void AudioEditor::setBeatsAndQuit(int beats)
{
  ab->setBeats(beats);
  window->hide();
}

/*
AudioBuffer* AudioEditor::getAudioBuffer()
{
  return ab;
}
*/

bool AudioEditor::shown()
{
  return window->shown();
}

AudioEditor::~AudioEditor()
{
  delete window;
}
