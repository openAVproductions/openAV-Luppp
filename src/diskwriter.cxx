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

#include "diskwriter.hxx"

#include "config.hxx"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <errno.h>

// to prompt the user of new filenames in case of overwrite
#include <FL/fl_ask.H>

#include "gui.hxx"
#include "hotkeymanager.hxx"
#include "gmastertrack.hxx"

#include "controller/genericmidi.hxx"

#include <sndfile.hh>

extern Gui* gui;

using namespace std;

DiskWriter::DiskWriter()
{
	sessionJson = cJSON_CreateObject();
	audioJson   = cJSON_CreateObject();

	// setup default controller name / author etc
	controllerInfo[CONTROLLER_NAME] = "no name";
	controllerInfo[CONTROLLER_AUTHOR] = "no author";
	controllerInfo[CONTROLLER_LINK] = "no link";

	sessionDir = getenv("HOME");
	sessionName = "session";
	foldersCreated = false;

	// create .config/openAV/luppp/ directory
	stringstream dotConfig;
	dotConfig << getenv("HOME") << "/.config/openAV/";
	int dotConfigDir  = mkdir( dotConfig.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( errno == EEXIST ) {
		//LUPPP_NOTE("dotConfigDir exists");
	} else if ( dotConfigDir ) {
		LUPPP_WARN("Error creating dotConfigDir: %s",  strerror(errno));
	} else {
		LUPPP_NOTE("Creating .config/openAV/ directory");
	}

	stringstream dotConfigLuppp;
	dotConfigLuppp << getenv("HOME") << "/.config/openAV/luppp";
	int dotConfigLupppDir  = mkdir( dotConfigLuppp.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( errno == EEXIST ) {
		//LUPPP_NOTE("dotConfigLupppDir exists");
	} else if ( dotConfigLupppDir ) {
		LUPPP_WARN("Error creating dotConfigLupppDir: %s",  strerror(errno));
	} else {
		LUPPP_NOTE("Creating .config/openAV/luppp directory");
	}

	stringstream dotConfigCtlr;
	dotConfigCtlr << getenv("HOME") << "/.config/openAV/luppp/controllers/";
	int dotConfigCtlrDir  = mkdir( dotConfigCtlr.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( errno == EEXIST ) {
		//LUPPP_NOTE("dotConfigCtlrDir exists");
	} else if ( dotConfigCtlrDir ) {
		LUPPP_WARN("Error creating dotConfigCtlrDir: %s",  strerror(errno));
	} else {
		LUPPP_NOTE("Creating .config/openAV/luppp directory");
	}
}

DiskWriter::~DiskWriter()
{
	// reset the cJSON objects
	cJSON_Delete( sessionJson );
	cJSON_Delete( audioJson   );
}

void DiskWriter::initialize(std::string path, std::string name )
{
	sessionName = name;
	sessionPath = path;

	/* If project dir doesn't exist yet, create it */
	int projectDir  = mkdir( path.c_str(),
	                         S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( errno == EEXIST ) {
		//LUPPP_NOTE("project dir %s exists, this is good", path.c_str());
	} else if ( projectDir ) {
		LUPPP_WARN("Error creating projectDir: %s", strerror(errno));
	} else {
		LUPPP_NOTE("Creating %s directory", path.c_str());
	}

	// write session.luppp JSON node to <path>/<sessionName>.luppp
	stringstream sessionDirStream;

	sessionDirStream << path;

	if ( !gui->getNsm() )
		sessionDirStream << "/" << sessionName << ".luppp";

	sessionDir = sessionDirStream.str();

	LUPPP_NOTE( "Creating session dir %s", sessionDir.c_str() );

	int sessionDirError = mkdir( sessionDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( sessionDirError ) {
		// handle by using different filename?
		LUPPP_WARN("Error creating session directory. Does the path: %s exist?",path.c_str());
	}

	stringstream audioDirStream;
	audioDirStream << sessionDir << "/audio";
	audioDir = audioDirStream.str();
	LUPPP_NOTE("Creating audio dir %s", audioDir.c_str() );

	int audioDirError  = mkdir( audioDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
	if ( audioDirError ) {
		LUPPP_WARN("Error creating sample directory");
	}

	foldersCreated = true;
}

std::string DiskWriter::getLastSaveName()
{
	return sessionName;
}

std::string DiskWriter::getLastSavePath()
{
	return sessionPath;
}

void DiskWriter::writeControllerInfo( CONTROLLER_INFO c, std::string s )
{
	controllerInfo[c] = s;
}

int DiskWriter::writeControllerFile( Controller* c )
{
	if ( c ) {
		LUPPP_NOTE("DiskWriter Controller* id: %i", c->getID() );
	} else {
		LUPPP_ERROR("DiskWriter Controller* passed NULL" );
		return LUPPP_RETURN_ERROR;
	}

	// check if controller of ID is actually a GenericMIDI controller
	GenericMIDI* g = dynamic_cast<GenericMIDI*>( c );

	if ( g ) {
		LUPPP_NOTE("Creating JSON for .ctlr file...");

		cJSON* controllerJson = cJSON_CreateObject();

		cJSON_AddItemToObject( controllerJson, "name",
		                       cJSON_CreateString( controllerInfo[CONTROLLER_NAME].c_str()   ) );
		cJSON_AddItemToObject( controllerJson, "author",
		                       cJSON_CreateString( controllerInfo[CONTROLLER_AUTHOR].c_str() ) );
		cJSON_AddItemToObject( controllerJson, "link",
		                       cJSON_CreateString( controllerInfo[CONTROLLER_LINK].c_str()   ) );

		// input bindings
		std::vector<Binding*> b = g->getMidiToAction();

		cJSON* inputBindings = cJSON_CreateArray();
		cJSON_AddItemToObject(controllerJson, "inputBindings", inputBindings );
		for(unsigned int i = 0; i < b.size(); i++ ) {
			// create binding
			cJSON* binding = cJSON_CreateObject();
			cJSON_AddItemToArray( inputBindings, binding );

			// add metadata to binding
			const char* actionName = Event::getPrettyName( b.at(i)->action );

			if ( actionName ) {
				cJSON_AddItemToObject( binding, "action", cJSON_CreateString( actionName ) );

				cJSON_AddNumberToObject( binding, "status", b.at(i)->status );
				cJSON_AddNumberToObject( binding, "data", b.at(i)->data   );

				// only add JSON elements if they're not the "invalid" defaults
				if ( b.at(i)->track != -2 )
					cJSON_AddNumberToObject( binding, "track", b.at(i)->track  );
				if ( b.at(i)->scene != -1 )
					cJSON_AddNumberToObject( binding, "scene", b.at(i)->scene  );
				if ( b.at(i)->send  != -1 )
					cJSON_AddNumberToObject( binding, "send", b.at(i)->send   );
				if ( b.at(i)->active!= -1 )
					cJSON_AddNumberToObject( binding, "active", b.at(i)->active );

				LUPPP_NOTE("Creating Binding: action %i == %s!", b.at(i)->action, actionName );
			} else {
				LUPPP_WARN("Binding action %i has no prettyName!", b.at(i)->action );
			}
		}

		// write the sample JSON node to <samplePath>/sample.cfg
		stringstream controllerCfgPath;
		controllerCfgPath << getenv("HOME") << "/.config/openAV/luppp/controllers/" << g->getName() << ".ctlr";

		ifstream infile( controllerCfgPath.str().c_str() );
		if ( infile.good() ) {
			// file exists: ask user overwrite or rename?
			//LUPPP_WARN("Controller filename exists: prompting user to overwrite y/n?");
			int action = fl_choice("Controller exists, action?", "Cancel", "Rename", "Overwrite");
			if ( action == 0 ) {
				// return OK, as user has chosen to cancel writing the file
				return LUPPP_RETURN_OK;
			} else if ( action == 1 ) {
				// rename here
				const char* name = fl_input("New name for .ctlr file:");
				if ( name ) {
					// clear the filename
					controllerCfgPath.str( "" );
					controllerCfgPath << getenv("HOME") << "/.config/openAV/luppp/controllers/" << name << ".ctlr";
					LUPPP_NOTE( "New .ctlr filename %s", controllerCfgPath.str().c_str() );
				} else {
					LUPPP_NOTE("No name entered for .ctlr file, canceling!");
					return LUPPP_RETURN_ERROR;
				}
			} else {
				// just overwrite the file, no action
			}


		}

		LUPPP_NOTE("Writing %s.ctlr file to disk", g->getName().c_str() );

		ofstream controllerCfgFile;
		controllerCfgFile.open ( controllerCfgPath.str().c_str() );
		controllerCfgFile << cJSON_Print( controllerJson );
		controllerCfgFile.close();
	} else {
		LUPPP_WARN("Invalid Controller pointer: cannot write %s as is not a GenericMIDI controller!", c->getName().c_str() );
		return LUPPP_RETURN_ERROR;
	}

	return LUPPP_RETURN_OK;
}

int DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab,
                                 const char *gui_path)
{
	stringstream path;
	if ( gui_path && strlen(gui_path) ) {
		printf("saving single buffer to %s\n", gui_path);
		path << gui_path;
	} else if ( foldersCreated ) {
		stringstream filename;
		filename << "t_" << track << "_s_" << scene << ".wav";

		// store the clip in clipData, we will write the session JSON for it in writeSession
		clipData.push_back( ClipData( track, scene, filename.str() ) );

		// add the AudioBuffer metadata to the sample JSON node
		cJSON* sampleClip = cJSON_CreateObject();
		cJSON_AddItemToObject(audioJson, filename.str().c_str(), sampleClip );
		cJSON_AddNumberToObject(sampleClip,"beats", ab->getBeats() );

		// get pretty name from GUI
		std::string clipName = gui->getTrack(track)->getClipSelector()->clipName( scene );
		cJSON_AddItemToObject  ( sampleClip, "name", cJSON_CreateString( clipName.c_str() ));

		// write the AudioBuffer contents to <path>/audio/  as  <name>.wav
		// or alternatively t_<track>_s_<scene>.wav

		path << audioDir << "/" << filename.str();
	} else {
		LUPPP_WARN("%s", "Session folders not created yet, while trying to write audioBuffers.");
		return LUPPP_RETURN_ERROR;
	}


	SndfileHandle outfile( path.str(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, 2, gui->samplerate );

	// FIXME: the size of the buffer is bigger than the audio contained in it:
	// calculate the length that needs saving using getBeats() * framesPerBeat
	if ( ab->getAudioFrames() > 0 ) {
		float frameBuf[ 2 ];
		for(int i=0; i<ab->getAudioFrames(); i++) {
			frameBuf[0] = ab->getDataL()[i];
			frameBuf[1] = ab->getDataR()[i];
			outfile.writef( frameBuf, 1);
		}
	} else {
		LUPPP_WARN("%s","Sample has zero samples");
	}

	return LUPPP_RETURN_OK;
}

void DiskWriter::writeMaster()
{
	// Master track stuff
	cJSON* masterTrack = cJSON_CreateObject();
	cJSON_AddItemToObject(sessionJson, "master", masterTrack );
	GMasterTrack* master = gui->getMasterTrack();

	cJSON_AddNumberToObject( masterTrack, "fader", master->getVolume()->value() );
	cJSON_AddNumberToObject( masterTrack, "bpm", gui->getMasterTrack()->getBpm() );
	// TODO add samplerate to session JSON
	//cJSON_AddNumberToObject( masterTrack, "samplerate", gui->getMasterTrack()->getBpm() );

	cJSON_AddNumberToObject( masterTrack, "inputVolume", gui->getMasterTrack()->getInputVolume()->value() );

	cJSON_AddNumberToObject( masterTrack, "inputToSndActive", gui->getMasterTrack()->getInputToSend()->value() );
	cJSON_AddNumberToObject( masterTrack, "inputToKeyActive", gui->getMasterTrack()->getInputToSidechainKey()->value() );
	cJSON_AddNumberToObject( masterTrack, "inputToMixActive", gui->getMasterTrack()->getInputToMix()->value() );

	cJSON_AddNumberToObject( masterTrack, "inputToSndVol", gui->getMasterTrack()->getInputToSendVol()->value() );
	cJSON_AddNumberToObject( masterTrack, "inputToXSide", gui->getMasterTrack()->getInputToXSide()->value() );
	cJSON_AddNumberToObject( masterTrack, "inputToMixVol", gui->getMasterTrack()->getInputToMixVol()->value() );

	// scene names
	Avtk::ClipSelector* clipSelector = master->getClipSelector();
	cJSON* sceneNames = cJSON_CreateArray();
	cJSON_AddItemToObject( masterTrack, "sceneNames", sceneNames );
	for(int i = 0; i < NSCENES; i++) {
		cJSON* sceneName = cJSON_CreateString( clipSelector->clipName(i).c_str() );
		cJSON_AddItemToArray( sceneNames, sceneName );
	}

}

int DiskWriter::writeSession()
{
	if ( !foldersCreated ) {
		LUPPP_WARN("%s", "Session folders not created yet, while trying to write session.");
		return LUPPP_RETURN_ERROR;
	}

	// add session metadata
	cJSON_AddItemToObject  ( sessionJson, "session", cJSON_CreateString( sessionName.c_str() ));

	cJSON_AddNumberToObject( sessionJson, "version_major", 1 );
	cJSON_AddNumberToObject( sessionJson, "version_minor", 0 );
	cJSON_AddNumberToObject( sessionJson, "version_patch", 0 );


	writeMaster();


	// add JSON "tracks" array
	cJSON* trackArray = cJSON_CreateArray();
	cJSON_AddItemToObject(sessionJson, "tracks", trackArray );

	// write tracks into JSON tracks array
	for(int t = 0; t < NTRACKS; t++) {
		cJSON* track = cJSON_CreateObject();
		cJSON_AddItemToArray( trackArray, track );

		// add track metadata: volumes, sends etc
		cJSON_AddNumberToObject( track, "ID", t );
		cJSON_AddStringToObject( track, "name", gui->getTrack(t)->bg.label() );

		cJSON_AddNumberToObject( track, "fader", gui->getTrack(t)->getVolume()->value() );
		cJSON_AddNumberToObject( track, "pan", gui->getTrack(t)->getPan());

		cJSON_AddNumberToObject( track, "sendAmount", gui->getTrack(t)->getSend()       );
		cJSON_AddNumberToObject( track, "sendActive", gui->getTrack(t)->getSendActive() );

		cJSON_AddNumberToObject( track, "jacksendAmount", gui->getTrack(t)->getJackSend()       );
		cJSON_AddNumberToObject( track, "jacksendActive", gui->getTrack(t)->getJackSendActivate() );

		cJSON_AddNumberToObject( track, "xsideAmount", gui->getTrack(t)->getXSide()      );
		cJSON_AddNumberToObject( track, "keyActive", gui->getTrack(t)->getKeyActive()  );

		// write clipData vector into clip placeholder
		cJSON* clips = cJSON_CreateArray();
		cJSON_AddItemToObject( track, "clips", clips );



		for(int s = 0; s < NSCENES; s++) {
			// add empty string to array
			cJSON* clip = cJSON_CreateString( "" );
			cJSON_AddItemToArray( clips, clip );

			// replace blank string if clip exists
			for(unsigned int i = 0; i < clipData.size(); i++) {
				if ( clipData.at(i).track == t &&
				     clipData.at(i).scene == s ) {
					cJSON* newClip = cJSON_CreateString( clipData.at(i).name.c_str() );
					cJSON_ReplaceItemInArray( clips, s, newClip );
				}
			}

		}

	}



	stringstream sessionLuppp;
	sessionLuppp << sessionDir << "/session.luppp";
	//c out << "Session dir: " << sessionDir.str() << "\n" << "Sample dir : " << audioDir.str() << endl;
	ofstream sessionFile;
	sessionFile.open ( sessionLuppp.str().c_str() );
	sessionFile << cJSON_Print( sessionJson );
	sessionFile.close();

	// write the sample JSON node to <samplePath>/sample.cfg
	stringstream audioCfg;
	audioCfg << audioDir << "/audio.cfg";

	ofstream audioCfgFile;
	audioCfgFile.open ( audioCfg.str().c_str() );
	audioCfgFile << cJSON_Print( audioJson );
	audioCfgFile.close();

	// clear the clipData, clean page for next save
	clipData.clear();


	// reset the cJSON objects
	cJSON_Delete( sessionJson );
	cJSON_Delete( audioJson   );

	sessionJson = cJSON_CreateObject();
	audioJson   = cJSON_CreateObject();


	return LUPPP_RETURN_OK;
}


void DiskWriter::writeDefaultConfigToUserHome()
{
	LUPPP_NOTE("Writing default preferences file.");

	cJSON* prfs = cJSON_CreateObject();

	// "__COMMENT__"         : "users home + <whatever it says here>"
	// "saveDirectory"       : "luppp"
	cJSON_AddItemToObject  ( prfs, "__COMMENT__",
	                         cJSON_CreateString("users home + <whatever it says here>") );
	cJSON_AddItemToObject  ( prfs, "saveDirectory", cJSON_CreateString( "luppp" ));


	// "__COMMENT__"         : "0 = LINEAR, 1 = SINC_FASTEST, 2 = SINC_BEST",
	// "resampleQuality"     : 2
	cJSON_AddItemToObject  ( prfs, "__COMMENT__",
	                         cJSON_CreateString("0 = LINEAR, 1 = SINC_FASTEST, 2 = SINC_BEST") );
	cJSON_AddNumberToObject( prfs, "resampleQuality", 1 );

	// "defaultControllers"  : [],
	cJSON* defCtrls = cJSON_CreateArray();
	cJSON_AddItemToObject( prfs, "defaultControllers", defCtrls );

	// per track send and return option
	cJSON_AddNumberToObject( prfs, "enablePerTrackSendReturns", 0 );

	cJSON* hotkeyAssignment = cJSON_CreateObject();

	// Iterate through the keyToGrid map and add each key-value pair to hotkeyAssignment
	for (const auto& pair : keyToGrid) {
		int key = pair.first;
		int id = pair.second.first;
		int clipNumber = pair.second.second;

		// Create an array for the id and clipNumber
		cJSON* array = cJSON_CreateArray();
		cJSON_AddItemToArray(array, cJSON_CreateNumber(id));
		cJSON_AddItemToArray(array, cJSON_CreateNumber(clipNumber));

		// Convert key to string and add to hotkeyAssignment
		cJSON_AddItemToObject(hotkeyAssignment, std::to_string(key).c_str(), array);
	}
	cJSON_AddItemToObject(prfs, "hotkeyAssignment", hotkeyAssignment);
	
	// test output on console
	// cout << endl << cJSON_Print( prfs ) << endl << endl;
	
	

	// write JSON to .config/openAV/luppp/luppp.prfs
	stringstream f;
	f << getenv("HOME") << "/.config/openAV/luppp/luppp.prfs";

	ofstream outFile;
	outFile.open ( f.str().c_str() );
	outFile << cJSON_Print( prfs );
	outFile.close();
}
