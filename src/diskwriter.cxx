
#include "diskwriter.hxx"

DiskWriter::DiskWriter()
{
};

void DiskWriter::writeAudioBuffer(int track, int scene, AudioBuffer* ab )
{
  // add the track / scene / name combo to audioConfig JSON node
  
  // write the AudioBuffer contents to <path>/samples/  as  <name>.wav
  // or alternatively t_<track>_s_<scene>.wav
  
}

void DiskWriter::writeSession( std::string path, std::string sessionName )
{
  // write the audioConfig JSON node to <path>/samples/sample.cfg
  
  
  // write session.luppp JSON node to <path>/<sessionName>.luppp
  
  
}
