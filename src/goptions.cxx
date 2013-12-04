

#include "goptions.hxx"

#include "eventhandler.hxx"

#include <FL/Fl_Button.H>

#include "controller/binding.hxx"
#include "controller/controller.hxx"
#include "controller/genericmidi.hxx"

#include "event.hxx"

#include "gui.hxx"
extern Gui* gui;

static void addControllerUiDsp(OptionsWindow* self, GenericMIDI* c)
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
  
  // tell the ControllerUI to add the bindings from this Controller*
  self->controllers.back()->setAuthor( c->getAuthor() );
  self->controllers.back()->setLink( c->getEmail() );
  self->controllers.back()->addBindings( c );
  
  self->tabs->redraw();
  
  // send to DSP side
  EventControllerInstance e(c);
  writeToDspRingbuffer( &e );
}

static void updateAuthorCB(Fl_Widget* w, void* data)
{
  ControllerUI* c = (ControllerUI*)data;
  const char* s = fl_input( "Author: ", "" );
  if ( s )
  {
    c->setAuthor( s );
  }
}

static void updateLinkCB(Fl_Widget* w, void* data)
{
  ControllerUI* c = (ControllerUI*)data;
  
  stringstream str;
  str << "xdg-open ";
  
  // add http:// if its not in the string
  std::string l =  c->getLink();
  if ( ( l.find("http") ) == std::string::npos )
    str << " http://";
  
  str << l;
  
  system( str.str().c_str() );
}

static void writeBindEnable(Fl_Widget* w, void* data)
{
  OptionsWindow* o = (OptionsWindow*) data;
  //LUPPP_NOTE("MIDI bind mode");
  
  Avtk::LightButton* l = (Avtk::LightButton*)w;
  l->value( !l->value() );
  
  int controllerID = -1; // waste?
  EventControllerBindingEnable e( controllerID, l->value() );
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
    return;
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
     case -1: /*printf("ERROR: %s\n", fnfc.errmsg());*/  break;  // ERROR
     case  1: /*printf("CANCEL\n");                  */  break;  // CANCEL
     default:
        //printf("Loading controller at %s\n", fnfc.filename());
        path = fnfc.filename();
        break;
  }
  
  if ( strcmp( path.c_str(), "" ) == 0 )
    return;
  
  LUPPP_NOTE("%s","ADD Controller cb");
  GenericMIDI* c = new GenericMIDI( path );
  
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
  ControllerUI* c = (ControllerUI*)data;
  
  LUPPP_NOTE("Writing controller %li, %s ID %i .ctlr to disk", c, c->name.c_str(), c->controllerID );
  
  // Set the Controller details in diskWriter, so it write it pretty
  gui->getDiskWriter()->writeControllerInfo( CONTROLLER_NAME  , c->name       );
  gui->getDiskWriter()->writeControllerInfo( CONTROLLER_AUTHOR, c->getAuthor());
  gui->getDiskWriter()->writeControllerInfo( CONTROLLER_LINK  , c->getLink()  );
  
  EventControllerInstanceGetToWrite e( c->controllerID );
  writeToDspRingbuffer( &e );
}


static void deleteBindingFromController(Fl_Widget* w, void* ud)
{
  ControllerUI* self = (ControllerUI*)ud;
  stringstream s;
  s << w->label();
  int tmp;
  s >> tmp;
  LUPPP_NOTE("CtlrID %i: Deleting binding with ID %i", self->controllerID, tmp );
  
  EventControllerBindingRemove e( self->controllerID, tmp );
  writeToDspRingbuffer( &e );
  
  // remove "this" widget (and its parent Pack) from the list of MIDI bindings:
  self->bindingsPack->remove( w->parent() );
  self->bindingsPack->redraw();
  self->scroll->redraw();
}


ControllerUI::ControllerUI(int x, int y, int w, int h, std::string n, int ID)
{
  name = n;
  
  widget = new Fl_Group( x, y, w, h, name.c_str());
  {
    // author / link
    authorLabel = new Avtk::Button( x + 5, y + 5, 190, 25, "Author?" );
    linkLabel   = new Avtk::Button( x + 7+ w/2, y + 5, 190, 25, "Link?" );
    
    authorLabel->label("Author?");
    authorLabel->label("Link?");
    
    authorLabel->callback( updateAuthorCB, this );
    linkLabel->callback( updateLinkCB, this );
    
    // binding / target
    targetLabelStat = new Fl_Box(x + 100,y + 32, 75, 25,"Target: ");
    targetLabel = new Fl_Box(x + 140,y + 32, 200, 25,"");
    bindEnable = new Avtk::LightButton(x + 5, y + 32, 100, 25, "Bind Enable");
    
    writeControllerBtn = new Avtk::Button( x + 5, y + h - 27, 100, 25, "Save" );
    removeController = new Avtk::Button( x + 110, y + h - 27, 100, 25, "Remove");
    
    scroll = new Fl_Scroll( x + 5, y + 82, 395, 265 );
    {
      bindingsPack = new Fl_Pack( x + 5, y + 82, w - 15, 270-10);
      bindingsPack->end();
      bindingsPack->spacing( 2 );
      bindingsPack->box(FL_DOWN_FRAME);
    }
    scroll->resizable( bindingsPack );
    scroll->box( FL_DOWN_FRAME );
    scroll->type( Fl_Scroll::VERTICAL_ALWAYS );
    scroll->end();
    
    widget->resizable( scroll );
  }
  widget->end();
  
  widget->redraw();
  
  // save the controller ID this ControllerUI represents
  controllerID = ID;
  LUPPP_NOTE("Controller %li ID on create %i", this, controllerID );
  
  //ctlrButton->callback( selectLoadController );
  bindEnable->callback( writeBindEnable, this );
  removeController->callback( removeControllerCB, this );
  writeControllerBtn->callback( writeControllerFile, this );
}

void OptionsWindow::setTarget(const char* n)
{
  for(unsigned int i = 0; i < controllers.size(); i++ )
  {
    controllers.at(i)->setTarget( n );
  }
}

void ControllerUI::setTarget( const char* n )
{
  target = n;
  targetLabel->label( target.c_str() );
  targetLabel->redraw();
}

void ControllerUI::setAuthor(std::string a)
{
  author = a;
  authorLabel->label( author.c_str() );
  authorLabel->redraw();
}

void ControllerUI::setLink(std::string e)
{
  link = e;
  linkLabel->label( link.c_str() );
  linkLabel->redraw();
}

void ControllerUI::setBindEnable( bool b )
{
  bindEnable->value( b );
}

void ControllerUI::addBinding( Binding* b )
{
  if ( b->action != EVENT_NULL )
  {
  // add individual bindings as they're made
    const char* tmp = Event::getPrettyName( b->action );
    if ( !tmp )
    {
#ifdef DEBUG_MIDI
      LUPPP_NOTE("new binding, action string returned NULL, action number %i ", b->action  );
#endif
      return;
    }
  }
  else
  {
    LUPPP_WARN("new binding, action: ==  EVENT_NULL" );
    return;
  }
  
  // push the bindingID onto the vector
  bindingID.push_back( b->ID );
  
  // create a horizontal pack, add that to the bindingsPack
  Fl_Pack* tmp = new Fl_Pack( 35, 35, 25, 25);
  {
    tmp->type( Fl_Pack::HORIZONTAL );
    tmp->spacing( 2 );
    
    stringstream s;
    
    if ( b->action == MASTER_VOL )
      s << "Master Volume";
    else if ( false )
      s << "stuff";
    else
      s << Event::getPrettyName( b->action );
    
    
    
    if (b->track != -2)
      s << " Track:" << b->track + 1;
    
    if (b->scene != -1)
      s << " Scene:" << b->scene + 1;
      
    if ( b->send == Event::SEND_POSTFADER )
      s << " Post-fader Send";
    if ( b->send == Event::SEND_XSIDE )
      s << " Sidechain Crossfade";
    if ( b->send == Event::SEND_KEY )
      s << " Sidechain Key";
    
    if ( b->active != -1)
    {
      if ( b->active == true )
        s << " On";
      if ( b->active == false )
      s << " Off";
    }
    
    // button to remove a binding, uses bindingsID vector to get unique number
    stringstream id;
    id << b->ID;
    Fl_Button* but = new Fl_Button(35, 35, 25, 25, strdup(id.str().c_str() ) );
    but->callback( deleteBindingFromController, this );
    but->redraw();
    
    Fl_Box* b = new Fl_Box(35, 35, 400, 25, strdup(s.str().c_str()) );
    b->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    b->redraw();
  }
  tmp->end();
  
  // push the binding to the UI pack, *and* store its binding ID in the same
  // array element in the bindingID vector. Used to remove bindings
  bindingsPack->add( tmp );
  
  bindingsPack->resize( bindingsPack->x(),bindingsPack->y(),bindingsPack->w(),bindingsPack->children() * 36 );
  bindingsPack->redraw();
  scroll->redraw();
  
  //LUPPP_NOTE("binding size %i %i", bindingsPack->w(), bindingsPack->h() );
}

void ControllerUI::addBindings( GenericMIDI* c )
{
  // FIXME: add binding to Avtk::Binding here
  std::vector<Binding*> bindingVector= c->getMidiToAction();
  
  for(unsigned int i = 0; i < bindingVector.size(); i++ )
  {
    addBinding( bindingVector.at(i) );
  }
}

ControllerUI::~ControllerUI()
{
  // FIXME CRITICAL! free UI component memory here
}

OptionsWindow::OptionsWindow()
{
  window = new Fl_Double_Window(400,400,"Options");
  
  window->set_non_modal();
  
  tabs = new Fl_Tabs(0, 0, 400, 400);
  
  window->resizable( tabs );
  
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

OptionsWindow::~OptionsWindow()
{
  delete newButton;
  delete loadButton;
  delete addGroup;
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
