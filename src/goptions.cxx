

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
  
  tabs = new Fl_Tabs(0, 0, 400, 300);
  
  int x, y, w, h;
  tabs->client_area( x, y, w, h, 25 );
  
  Fl_Group* bindings = new Fl_Group( x, y, w, h, "Binding");
  {
    targetLabel = new Avtk::Button(x + 105, y + 5, 100, 25, "Target: ");
    bindEnable = new Avtk::LightButton(x + 5, y + 5, 100, 25, "Bind Enable");
  }
  bindings->end();

  Fl_Group* controllers = new Fl_Group( x, y, w, h, "Controllers");
  controllers->hide();
  {
    ctlrButton = new Avtk::Button(x + 5, y + 5, 100, 25, "Add Controller");
  }
  controllers->end();
  
  tabs->end();
  
  //ctlrButton->callback( selectLoadController );
  bindEnable->callback( writeBindEnable, this );
  
  window->end();
}

void OptionsWindow::setTarget(const char* t)
{
  target = t;
  printf("%s\n", target.c_str() );
  targetLabel->setLabel( t );
}

void OptionsWindow::show()
{
  window->show();
}

void OptionsWindow::setBindEnable(bool e)
{
  bindEnable->value( e );
}


