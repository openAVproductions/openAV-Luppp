

#include "goptions.hxx"

#include "eventhandler.hxx"

#include "controller/binding.hxx"
#include "controller/controller.hxx"
#include "controller/genericmidi.hxx"

#include "gui.hxx"
extern Gui* gui;

static void writeBindEnable(Fl_Widget* w, void* data)
{
  OptionsWindow* o = (OptionsWindow*) data;
  //LUPPP_NOTE("MIDI bind mode");
  
  Avtk::LightButton* l = (Avtk::LightButton*)w;
  l->value( !l->value() );
  
  EventControllerBindingEnable e( l->value() );
  writeToDspRingbuffer( &e );
  
  if ( l->value() < 0.5 )
  {
    o->setTarget("");
  }
}

static void selectLoadController(Fl_Widget* w, void*)
{
  // FIXME: refactor
  string path;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a controller definition");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fnfc.filter("Controllers\t*.ctlr");
  fnfc.directory( getenv("HOME") ); // default directory to use
  // Show native chooser
  switch ( fnfc.show() ) {
     case -1: printf("ERROR: %s\n", fnfc.errmsg());    break;  // ERROR
     case  1: printf("CANCEL\n");                      break;  // CANCEL
     default: printf("Loading controller at %s\n", fnfc.filename());
        path = fnfc.filename();
        break;
  }
  
  if ( strcmp( path.c_str(), "" ) == 0 )
    return;
  
  LUPPP_NOTE("%s","ADD Controller cb");
  Controller* c = new GenericMIDI( path );
  
  if ( c->status() == Controller::CONTROLLER_OK )
  {
    EventControllerInstance e(c);
    writeToDspRingbuffer( &e );
  }
  else
  {
    LUPPP_ERROR("Controller initialization failed!");
  }
}

static void writeControllerFile(Fl_Widget* w, void* data)
{
  OptionsWindow* o = (OptionsWindow*) data;
  
  // FIXME: Controller ID hardcoded
  EventControllerInstanceGetToWrite e( 1 );
  writeToDspRingbuffer( &e );
}


OptionsWindow::OptionsWindow()
{
  window = new Fl_Double_Window(400,400,"Options");
  
  tabs = new Fl_Tabs(0, 0, 400, 400);
  
  int x, y, w, h;
  tabs->client_area( x, y, w, h, 25 );
  
  Fl_Group* bindingGroup = new Fl_Group( x, y, w, h, "Binding");
  {
    targetLabelStat = new Fl_Box(x + 100,y + 5, 75, 25,"Target: ");
    targetLabel = new Fl_Box(x + 140,y + 5, 200, 25,"");
    bindEnable = new Avtk::LightButton(x + 5, y + 5, 100, 25, "Bind Enable");
    
    writeControllerBtn = new Avtk::Button( x + 5, y + 275, 100, 25, "Save" );
    
    Fl_Scroll* s = new Fl_Scroll( x + 5, y + 35, 400, 180 );
    bindings = new Avtk::Bindings( x + 5, y + 35, 400, 200 );
    s->end();
  }
  bindingGroup->end();

  Fl_Group* controllers = new Fl_Group( x, y, w, h, "Controllers");
  controllers->hide();
  {
    ctlrButton = new Avtk::Button(x + 5, y + 5, 100, 25, "Add Controller");
  }
  controllers->end();
  
  tabs->end();
  
  ctlrButton->callback( selectLoadController );
  bindEnable->callback( writeBindEnable, this );
  writeControllerBtn->callback( writeControllerFile, this );
  
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

void OptionsWindow::addBinding( Binding* b )
{
  bindings->add( b );
}

