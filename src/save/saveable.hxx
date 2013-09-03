
#ifndef LUPPP_SAVEABLE_H
#define LUPPP_SAVEABLE_H

/** SaveAble
 * This class is inherited from by all classes that can save thier state.
 * The virtual save() function is overriden, and called when a save action is
 * triggered.
 * 
 * This flexibility allows eg LooperClips to request buffers to transer audio
 * data into the GUI thread for disk serialization, and scales for future 
 * classes which also need to request memory in order to save in a RT safe way.
**/
class SaveAble
{
  public:
    SaveAble();
    
    /// this function is called when the user initiates a save action
    virtual void save();
};

#endif // LUPPP_SAVEABLE_H

