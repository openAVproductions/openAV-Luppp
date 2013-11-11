

#include "goptions.hxx"

#include "eventhandler.hxx"

#include "controller/binding.hxx"
#include "controller/controller.hxx"
#include "controller/genericmidi.hxx"

#include "gui.hxx"
extern Gui* gui;

static void addControllerUiDsp(OptionsWindow* self, Controller* c)
{
  // add the controller to the UI
  int x, y, w, h;
  self->tabs->client_area( x, y, w, h, 25 );
  self->controllers.push_back( new ControllerUI( x, y, w, h, c->getName().c_str(), c->getID() ) );
  
  // store the pointer to the options window: needed to make remove button work
  self->controllers.back()->optionsWindow = self;
  
  LUPPP_NOTE("Added controller %s, ID %i", c->getName().c_str(), c->getID() );

  // add widget before "add" button
  self->tabs->insert( *self->controllers.back()->widget, self->addGroup );
  
  self->tabs->redraw();
  
  // send to DSP side
  EventControllerInstance e(c);
  writeToDspRingbuffer( &e );
}

static void writeBindEnable(Fl_Widget* w, void* data)
{
  OptionsWindow* o = (OptionsWindow*) data;
  //LUPPP_NOTE("MIDI bind mode");
  
  Avtk::LightButton* l = (Avtk::LightButton*)w;
  l->value( !l->value() );
  
  EventControllerBindingEnable e( l->value() );
  writeToDspRingbuffer( &e );
}

static void removeControllerCB(Fl_Widget* w, void* data)
{
  ControllerUI* self = (ControllerUI*)data;
  
  // Remove UI tab for that controller
  // should return "tabs" from OptionsWindow
  self->optionsWindow->tabs->remove( self->widget );
  self->optionsWindow->tabs->redraw();
  
  // FIXME: confirm action here?
  
  //LUPPP_NOTE("Removing controllerID %i", self->controllerID );
  EventControllerInstanceRemove e( self->controllerID );
  writeToDspRingbuffer( &e );
  
  delete self;
}

static void addNewController(Fl_Widget* w, void* ud)
{
  OptionsWindow* self = (OptionsWindow*)ud;
  LUPPP_NOTE("%s","ADD Controller cb");
  
  GenericMIDI* c = 0;
  
  const char* name = fl_input( "Controller name: ", "" );
  if ( name )
  {
    c = new GenericMIDI( 0, name);
  }
  else
  {
    c = new GenericMIDI();
  }
  
  
  if ( c->status() == Controller::CONTROLLER_OK )
  {
    addControllerUiDsp( self, c );
  }
  else
  {
    LUPPP_ERROR("Controller initialization failed!");
  }
}



static void selectLoadController(Fl_Widget* w, void* data)
{
  OptionsWindow* self = (OptionsWindow*)data;
  
  // FIXME: refactor
  string path;
  Fl_Native_File_Chooser fnfc;
  fnfc.title("Pick a controller definition");
  fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fnfc.filter("Controllers\t*.ctlr");
  
  stringstream s;
  s << getenv("HOME") << "/.config/openAV/luppp/controllers/";
  fnfc.directory( s.str().c_str() ); // default directory to use
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
    addControllerUiDsp( self, c );
  }
  else
  {
    LUPPP_ERROR("Controller initialization failed!");
  }
}



static void writeControllerFile(Fl_Widget* w, void* data)
{
  // a pointer to the controllerID int is passed as data
  ControllerUI* c = (ControllerUI*)data;
  
  LUPPP_NOTE("Writing controller %li, %s ID %i .ctlr to disk", c, c->name, c->controllerID );
  
  // FIXME: Controller ID hardcoded
  EventControllerInstanceGetToWrite e( 2 );
  writeToDspRingbuffer( &e );
}


ControllerUI::ControllerUI(int x, int y, int w, int h, std::string n, int ID)
{
  name = strdup(n.c_str());
  target = 0;
  
  widget = new Fl_Group( x, y, w, h, name);
  {
    targetLabelStat = new Fl_Box(x + 100,y + 5, 75, 25,"Target: ");
    targetLabel = new Fl_Box(x + 140,y + 5, 200, 25,"");
    bindEnable = new Avtk::LightButton(x + 5, y + 5, 100, 25, "Bind Enable");
    
    writeControllerBtn = new Avtk::Button( x + 5, y + h - 27, 100, 25, "Save" );
    //ctlrButton = new Avtk::Button(x + 110, y + 275, 100, 25, "Load");
    removeController = new Avtk::Button(x + 110, y + h - 27, 100, 25, "Remove");
    
    Fl_Scroll* s = new Fl_Scroll( x + 5, y + 35, 400, 180 );
    bindings = new Avtk::Bindings( x + 5, y + 35, 398, 10 );
    s->end();
  }
  widget->end();
  
  // save the controller ID this ControllerUI represents
  controllerID = ID;
  LUPPP_NOTE("Controller %li ID on create %i", this, controllerID );
  
  //ctlrButton->callback( selectLoadController );
  bindEnable->callback( writeBindEnable, this );
  removeController->callback( removeControllerCB, this );
  writeControllerBtn->callback( writeControllerFile, this );
}

void ControllerUI::setTarget(const char* n)
{
  if ( target )
    free (target);
  target = strdup(n);
  
  targetLabel->label( target );
  targetLabel->redraw();
}

void ControllerUI::setBindEnable( bool b )
{
  bindEnable->value( b );
}
void ControllerUI::addBinding( Binding* b )
{
  // FIXME: add binding to Avtk::Binding here
}

ControllerUI::~ControllerUI()
{
  // FIXME CRITICAL! free UI component memory here
}

OptionsWindow::OptionsWindow()
{
  window = new Fl_Double_Window(400,400,"Options");
  
  tabs = new Fl_Tabs(0, 0, 400, 400);
  
  int x, y, w, h;
  tabs->client_area( x, y, w, h, 25 );
  
  addGroup = new Fl_Group(x,y,w,h,"Add");
  {
    newButton = new Avtk::Button( x+2, y+2, w-4, 30, "New Generic MIDI");
    loadButton = new Avtk::Button( x+2, y+2+32, w-4, 30, "Load Generic MIDI");
  }
  addGroup->end();
  tabs->end();
  
  newButton ->callback( addNewController, this );
  loadButton->callback( selectLoadController, this );
  
  window->end();
}

OptionsWindow::~OptionsWindow
{
  delete newButton;
  delete loadButton;
  delete addgroup;
  delete tabs;
  delete window;
}

void OptionsWindow::show()
{
  window->show();
}

void OptionsWindow::hide()
{
  window->hide();
}

ControllerUI* OptionsWindow::getControllerUI(int id)
{
  for(unsigned int i = 0; i < controllers.size(); i++ )
  {
    if ( controllers.at(i)->controllerID == id )
    {
      return controllers.at(i);
    }
  }
  
  // error: controller not found!
  return 0;
}
