
#ifndef LUPPP_SAVE_H
#define LUPPP_SAVE_H

#include <vector>

#include "saveable.hxx"

/** Save
 * This class keeps a list of all SaveAble instances, and calls save on them
 * when save() is called on this class.
**/
class Save
{
  public:
    Save();
    
    void save();
    
    /// called when each part is finished, and we can flush the metadata to disk
    void finish();
    
    void registerSaveable(SaveAble* s);
    
    int getNumSaveables()
    {
      return saveables.size();
    }
    
  private:
    std::vector<SaveAble*> saveables;
};

#endif // LUPPP_SAVE_H
