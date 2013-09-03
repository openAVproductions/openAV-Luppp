
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
    
    void registerSaveable(SaveAble* s);
    
  private:
    std::vector<SaveAble*> saveables;
};

#endif // LUPPP_SAVE_H
