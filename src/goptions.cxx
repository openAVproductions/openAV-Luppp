

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
    targetLabel = new Fl_Box(x + 110,y + 5, 200, 25,"");
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
  if ( target )
    free (target);
  target = strdup(t);
  
  targetLabel->label( target );
  targetLabel->redraw();
  
  //LUPPP_NOTE("New Target %s\n", target );
}

void OptionsWindow::show()
{
  window->show();
}

void OptionsWindow::setBindEnable(bool e)
{
  bindEnable->value( e );
  setTarget("");
}


