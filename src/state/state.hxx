
#ifndef LUPPP_STATE_H
#define LUPPP_STATE_H

#include <vector>

#include "stately.hxx"

/** State
 * This class keeps a list of all Stately instances, and calls save on them
 * when save() is called on this class.
**/
class State
{
  public:
    State();
    
    /// called to initiate a save on all Statelys
    void save();
    
    /// called to initiate a reset on all Statelys
    void reset();
    
    /// called when each part is finished, and we can flush the metadata to disk
    void finish();
    
    void registerStately(Stately* s);
    
    int getNumStatelys();
    
  private:
    std::vector<Stately*> statelys;
};

#endif // LUPPP_STATE_H
