
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
    
    /// this function being called resets the state of the instance to blank
    virtual void reset();
    
    /// this function is called when the user initiates a save action
    virtual void save();
    
    /// this function *must* be called by each sub-class when it is *finished*
    /// its save action. Once each Saveable is done, the final save is OK-ed.
    static void done();
  
  private:
    static int savesDone;
};

#endif // LUPPP_SAVEABLE_H

