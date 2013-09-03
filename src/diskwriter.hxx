
#ifndef LUPPP_DISK_WRITER_H
#define LUPPP_DISK_WRITER_H

#include <string>

#include "cjson/cJSON.h"

class AudioBuffer;

/** DiskWriter
 * This class writes soundfiles to disk, and keeps track of which filename was
 * in which track/scene combo in the grid. This metadata is then written to the
 * <sessionName>.luppp file.
**/
class DiskWriter
{
  public:
    DiskWriter();
    
    /// writes a single audio buffer to disk
    void writeAudioBuffer(int track, int scene, AudioBuffer* ab );
    
    void writeSession( std::string path, std::string sessionName );
  
  private:
    cJSON* session;
    cJSON* audioConfig;
};

#endif // LUPPP_DISK_WRITER_H
