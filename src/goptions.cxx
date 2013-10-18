

#include "goptions.hxx"

#include "eventhandler.hxx"



static void writeBindEnable(Fl_Widget* w, void* data)
{
  OptionsWindow* o = (OptionsWindow*) data;
  //LUPPP_NOTE("MIDI bind mode");
  
  Avtk::LightButton* l = (Avtk::LightButton*)w;
  l->value( !l->value() );
  
  EventControllerBindEnable e( l->value() );
  writeToDspRingbuffer( &e );
}


OptionsWindow::OptionsWindow()
{
  window = new Fl_Double_Window(400,300,"Options");
  
  ctlrButton = new Avtk::Button(5, 25, 200, 20, "Add Controller");
  bindEnable = new Avtk::LightButton(5, 55, 200, 20, "Bind Enable");
  
  //ctlrButton->callback( selectLoadController );
  bindEnable->callback( writeBindEnable, this );
  
  window->end();
}

void OptionsWindow::show()
{
  window->show();
}

void OptionsWindow::setBindEnable(bool e)
{
  bindEnable->value( e );
}


