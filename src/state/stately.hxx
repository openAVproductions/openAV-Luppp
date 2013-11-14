
#ifndef LUPPP_STATELY_H
#define LUPPP_STATELY_H

/** Stately
 * This class is inherited from by all classes that have state.
 * 
 * save() should be overriden if the object needs to save its state
 * reset() should be overriden if the object can clear its state
 * 
 * The flexibility allows eg LooperClips to request buffers to tranfser audio
 * data into the GUI thread for disk serialization, and scales for future 
 * classes which also need to request memory in order to save in a RT safe way.
**/
class Stately
{
  public:
    Stately();
    
    /// this function being called resets the state of the instance to blank
    virtual void reset();
    
    /// this function is called when the user initiates a save action
    virtual void save();
    
    /// this function *must* be called by each sub-class when it is *finished*
    /// a successful save action. Once each Stately is done, the final save is OK-ed.
    static void success();
    
    /// this function notes that a stately could *not* successfully save: buffer
    /// size mismatch in LooperClip for example.
    static void error(const char* errorString);
  
  private:
    /// holds the amount of successful / error-full saves. Used by success()
    /// and error()
    static int saveSuccess;
    static int saveErrors;
    
    static void checkCompletedSave();
};

#endif // LUPPP_STATELY_H

