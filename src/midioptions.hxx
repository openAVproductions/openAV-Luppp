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

#ifndef LUPPP_OPTIONS_H
#define LUPPP_OPTIONS_H

#include "config.hxx"

#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>

#include "controller/genericmidi.hxx"

#include "avtk/bindings.h"
#include "avtk/avtk_button.h"
#include "avtk/avtk_light_button.h"

class Binding;
class MidiOptionsWindow;

/// contains UI elements to represent one controller
class ControllerUI
{
public:
	ControllerUI( int x, int y, int w, int h, std::string name,int id);
	~ControllerUI();

	void setAuthor(std::string author);
	void setLink (std::string link );
	std::string getAuthor()
	{
		return author;
	}
	std::string getLink()
	{
		return link;
	}

	void setTarget(const char* n);
	void setBindEnable( bool b );

	void addBinding( Binding* b );
	void addBindings( GenericMIDI* c );

	// the ControllerID this UI class represents
	int controllerID;

	// for adding to GOptions tabs
	Fl_Group* widget;

	std::string name;

	MidiOptionsWindow* optionsWindow;

	// public to redraw when removing from static
	Fl_Scroll* scroll;
	Fl_Pack* bindingsPack;

private:
	// bindings
	std::string target;
	std::string author;
	std::string link;

	Avtk::Button* authorLabel;
	Avtk::Button* linkLabel;

	std::vector<int> bindingID;

	Fl_Box* targetLabel;
	Fl_Box* targetLabelStat;
	//Avtk::Bindings* bindings;
	Avtk::LightButton* bindEnable;
	Avtk::Button* removeController;
	Avtk::Button* writeControllerBtn;
};

class MidiOptionsWindow
{
public:
	MidiOptionsWindow();
	~MidiOptionsWindow();

	void show();
	void hide();

	void setTarget(const char* n);
	ControllerUI* getControllerUI(int id);

	// public for static methods only
	Fl_Tabs* tabs;
	std::vector<ControllerUI*> controllers;
	Fl_Group* addGroup;
#ifdef BUILD_TESTS
	int runTests();
#endif

private:
	Fl_Double_Window* window;
	Avtk::Button* newButton;
	Avtk::Button* loadButton;
};

#endif // LUPPP_OPTIONS_H
