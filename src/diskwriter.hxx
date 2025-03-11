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

#ifndef LUPPP_DISK_WRITER_H
#define LUPPP_DISK_WRITER_H

#include <string>
#include <vector>

#include "cjson/cJSON.h"

class AudioBuffer;
class Controller;

enum CONTROLLER_INFO {
	CONTROLLER_NAME,
	CONTROLLER_AUTHOR,
	CONTROLLER_LINK,
	CONTROLLER_INFO_SIZE,
};

/// To hold data about loaded clips until we write the JSON out
class ClipData
{
public:
	ClipData(int tr, int sc, std::string na) :
		track(tr), scene(sc), name(na) {}
	int track;
	int scene;
	std::string name;
};

/** DiskWriter
 * This class writes soundfiles to disk, and keeps track of which filename was
 * in which track/scene combo in the grid. This metadata is then written to the
 * <sessionName>.luppp file.
**/
class DiskWriter
{
public:
	DiskWriter();
	~DiskWriter();

	/// sets up session write path etc
	void initialize( std::string path, std::string sessionName );

	/// writes a single audio buffer to disk for saving whole state.
	// When gui_path is set, it only saves a single AB* to that path
	int writeAudioBuffer(int track, int scene, AudioBuffer* ab,
	                     const char* gui_path = 0);

	/// flush the JSON to disk, finalizing the save
	int writeSession();

	std::string getLastSaveName();
	std::string getLastSavePath();

	/// sets a piece of info to be written to the controller
	void writeControllerInfo( CONTROLLER_INFO c, std::string s );
	/// writes a controller definition .ctlr JSON file from a GenericMIDI instance
	int writeControllerFile( Controller* c );

	/// writes default config file to users home if it doesn't exist
	void writeDefaultConfigToUserHome();

#ifdef BUILD_TESTS
	int runTests();
#endif

private:
	cJSON* sessionJson = nullptr;
	cJSON* audioJson = nullptr;

	bool foldersCreated;
	std::string sessionName;
	std::string sessionPath;
	std::string audioDir;
	std::string sessionDir;

	std::vector<ClipData> clipData;

	// convienice functions for code separation
	void writeMaster();
	std::string controllerInfo[CONTROLLER_INFO_SIZE];
};

#endif // LUPPP_DISK_WRITER_H
