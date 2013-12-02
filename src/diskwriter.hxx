
#ifndef LUPPP_DISK_WRITER_H
#define LUPPP_DISK_WRITER_H

#include <string>
#include <vector>

#include "cjson/cJSON.h"

class AudioBuffer;
class Controller;

enum CONTROLLER_INFO
{
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
    
    /// sets up session write path etc
    void initialize( std::string path, std::string sessionName );
    
    /// writes a single audio buffer to disk
    int writeAudioBuffer(int track, int scene, AudioBuffer* ab );
    
    /// flush the JSON to disk, finalizing the save
    int writeSession();
    
    std::string getLastSaveName();
    std::string getLastSavePath();
    
    /// sets a piece of info to be written to the controller
    void writeControllerInfo( CONTROLLER_INFO c, std::string s );
    /// writes a controller definition .ctlr JSON file from a GenericMIDI instance
    int writeControllerFile( Controller* c );
    
    
#ifdef BUILD_TESTS
    int runTests();
#endif
  
  private:
    cJSON* sessionJson;
    cJSON* audioJson;
    
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
