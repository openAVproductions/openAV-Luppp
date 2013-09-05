
#ifndef LUPPP_DISK_READER_H
#define LUPPP_DISK_READER_H

#include <string>

#include "cjson/cJSON.h"

class AudioBuffer;

/** DiskReader
 * This class reads a previously saved session from disk, restoring Luppp's
 * internal state to that of when the save took place.
 * 
 * The directory <sessionDir> is the main point of loading. The user selects 
 * that directory to load the session from. <sessionDir>.luppp is the name of
 * the session file: it contains all info about the session: Volumes, loaded
 * samples etc.
 * 
 * Samples are read from the directory <sessionDir>/samples, in which there is a
 * sample.cfg file, which can be parsed to get sample metadata.
**/
class DiskReader
{
  public:
    DiskReader();
    
    /// loads a sample into a new AudioBuffer, returning the buffer
    void loadSample( int track, int scene, std::string path );
    
    /// reads a session from disk, parsing and restoring state
    void readSession( std::string path );
  
  private:
    cJSON* session;
    
    std::string sessionName;
    std::string sessionPath;
    
    // convinience functions
    void readGrid();
    void readMaster();
};

#endif // LUPPP_DISK_READER_H
