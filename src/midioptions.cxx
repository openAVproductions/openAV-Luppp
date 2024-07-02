/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "midioptions.hxx"

#include "eventhandler.hxx"

#include <FL/Fl_Button.H>

#include "controller/binding.hxx"
#include "controller/controller.hxx"
#include "controller/genericmidi.hxx"

#include "event.hxx"

#include "gui.hxx"
extern Gui* gui;

static void addControllerUiDsp(MidiOptionsWindow* self, GenericMIDI* c)
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
	if ( s ) {
		c->setAuthor( s );
	}
}

static void updateLinkCB(Fl_Widget* w, void* data)
{
	ControllerUI* c = (ControllerUI*)data;
	std::string l = c->getLink();
	// Check if the link is empty
    if (l.empty()) {
        // Prompt for the link
        const char* newLink = fl_input("URL to your page (optional):", "");
        if (newLink) {
            // Set the new link
            c->setLink(newLink);
            l = newLink; // Update the link variable
        }
    } else {
		stringstream str;
		str << "xdg-open ";

		// add http:// if its not in the string
		std::string l =  c->getLink();
		if ( ( l.find("http") ) == std::string::npos )
			str << " http://";

		str << l;

		int ret = system( str.str().c_str() );
		/* if it fails it fails... (void) to mute clang warning */
		(void)ret;
	}

    // the link is still empty :(
    if (l.empty()) {
        LUPPP_NOTE("No link provided.");
        return;
    }


}

static void exportMidiBindingCB(Fl_Widget* w, void* data)
{
	updateAuthorCB(w, data);
	updateLinkCB(w, data);
}

static void writeBindEnable(Fl_Widget* w, void* data)
{
	MidiOptionsWindow* o = (MidiOptionsWindow*) data;
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
	MidiOptionsWindow* self = (MidiOptionsWindow*)ud;
	LUPPP_NOTE("%s","ADD Controller cb");

	GenericMIDI* c = 0;

	const char* name = fl_input( "MIDI Controller Name: ", "" );
	if ( name ) {
		c = new GenericMIDI( 0, name);
	} else {
		return;
	}


	if ( c->status() == Controller::CONTROLLER_OK ) {
		addControllerUiDsp( self, c );
	} else {
		LUPPP_ERROR("Controller initialization failed!");
	}
}



static void selectLoadController(Fl_Widget* w, void* data)
{
	MidiOptionsWindow* self = (MidiOptionsWindow*)data;

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
	case -1: /*printf("ERROR: %s\n", fnfc.errmsg());*/
		break;  // ERROR
	case  1: /*printf("CANCEL\n");                  */
		break;  // CANCEL
	default:
		//printf("Loading controller at %s\n", fnfc.filename());
		path = fnfc.filename();
		break;
	}

	if ( strcmp( path.c_str(), "" ) == 0 )
		return;

	//LUPPP_NOTE("%s","ADD Controller cb");
	GenericMIDI* c = new GenericMIDI( path );

	if ( c->status() == Controller::CONTROLLER_OK ) {
		addControllerUiDsp( self, c );
	} else {
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
	if (n == "") {
		name = "Untitled";
	}

	widget = new Fl_Group(x, y, w, h, name.c_str());
	{
		// Binding / Target
		Fl_Box* bindingDescription = new Fl_Box(x + 10, y + 15, w - 20, 25, "Click 'Start Binding Mode' and then click a control to target it. After a target is selected, send a MIDI event to bind it.");
		bindingDescription->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

		bindEnable = new Avtk::LightButton(x + 10, y + 45, 140, 25, "Start Binding Mode");
		bindEnable->tooltip("Activate binding mode and click a control to map it to the target");

		targetLabelStat = new Fl_Box(x + 160, y + 45, 60, 25, "Target:");
		targetLabel = new Fl_Box(x + 230, y + 45, 160, 25, "");

		// Save / Remove Buttons
		writeControllerBtn = new Avtk::Button(x + 10, y + h - 40, 180, 25, "Save Bindings");
		removeController = new Avtk::Button(x + 210, y + h - 40, 180, 25, "Remove Bindings");
		exportControllerBtn = new Avtk::Button(x + 410, y + h - 40, 180, 25, "Export Bindings");

		// Author Label
		authorLabel = new Fl_Button(x + 610, y + h - 40, 180, 25, "");
		authorLabel->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
		authorLabel->box(FL_NO_BOX); // Makes the button look like a label
		authorLabel->labelfont(FL_HELVETICA_ITALIC);
		authorLabel->labelsize(12);
		authorLabel->labelcolor(FL_WHITE);
		authorLabel->callback(updateLinkCB, this);
		
		exportControllerBtn->callback(exportMidiBindingCB, this);

		// Scroll and Pack for Bindings
		scroll = new Fl_Scroll(x + 5, y + 85, 800, 245);
		{
			bindingsPack = new Fl_Pack(x + 5, y + 85, w - 15, 250-10);
			bindingsPack->end();
			bindingsPack->spacing(2);
			bindingsPack->box(FL_DOWN_FRAME);
		}
		scroll->resizable(bindingsPack);
		scroll->box(FL_DOWN_FRAME);
		scroll->type(Fl_Scroll::VERTICAL_ALWAYS);
		scroll->end();

		widget->resizable(scroll);
		widget->end();
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

void MidiOptionsWindow::setTarget(const char* n)
{
	for(unsigned int i = 0; i < controllers.size(); i++ ) {
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
	if (a != "") {
		author = a;
		authorMessage = "Made by " + author;
		
		authorLabel->label( authorMessage.c_str() );
		authorLabel->redraw();
	}
	
}

void ControllerUI::setLink(std::string e)
{
	link = e;	
	authorLabel->redraw();
}

void ControllerUI::setBindEnable( bool b )
{
	bindEnable->value( b );
}

void ControllerUI::addBinding( Binding* b )
{
	if ( b->action != EVENT_NULL ) {
		// add individual bindings as they're made
		const char* tmp = Event::getPrettyName( b->action );
		if ( !tmp ) {
#ifdef DEBUG_MIDI
			LUPPP_NOTE("new binding, action string returned NULL, action number %i ", b->action  );
#endif
			return;
		}
	} else {
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

		if ( b->active != -1) {
			if ( b->active == true )
				s << " On";
			if ( b->active == false )
				s << " Off";
		}

		// button to remove a binding, uses bindingsID vector to get unique number
		stringstream id;
		id << b->ID;
		char *str = strdup(id.str().c_str());
		Fl_Button* but = new Fl_Button(35, 35, 25, 25, str );
		if(!but)
			free(str);
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
	std::vector<Binding*> bindingVector= c->getMidiToAction();

	for(unsigned int i = 0; i < bindingVector.size(); i++ ) {
		addBinding( bindingVector.at(i) );
	}
}

ControllerUI::~ControllerUI()
{

	// free all binding resources here:


	delete authorLabel;

	delete targetLabel;
	delete targetLabelStat;
	delete bindEnable;
	delete removeController;
	delete writeControllerBtn;
}

MidiOptionsWindow::MidiOptionsWindow()
{
	int windowW = 815;
	int windowH = 400;
	window = new Fl_Double_Window(windowW, windowH, "MIDI Options");

	window->set_non_modal();

	tabs = new Fl_Tabs(0, 0, windowW, windowH);

	window->resizable( tabs );

	int x, y, w, h;
	tabs->client_area( x, y, w, h, 25 );

	addGroup = new Fl_Group(x,y,w,h,"New");
	{
		newButton = new Avtk::Button( x+2, y+2, w-4, 30, "Create New MIDI Bindings");
		loadButton = new Avtk::Button( x+2, y+2+32, w-4, 30, "Load Existing MIDI Bindings");
	}
	addGroup->end();
	tabs->end();

	newButton ->callback( addNewController, this );
	loadButton->callback( selectLoadController, this );

	window->end();
}

MidiOptionsWindow::~MidiOptionsWindow()
{
	delete newButton;
	delete loadButton;
	delete addGroup;
	delete tabs;
	delete window;
}

void MidiOptionsWindow::show()
{
	window->show();
}

void MidiOptionsWindow::hide()
{
	window->hide();
}

ControllerUI* MidiOptionsWindow::getControllerUI(int id)
{
	for(unsigned int i = 0; i < controllers.size(); i++ ) {
		if ( controllers.at(i)->controllerID == id ) {
			return controllers.at(i);
		}
	}

	// error: controller not found!
	return 0;
}
