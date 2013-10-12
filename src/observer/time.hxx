
#ifndef LUPPP_TIME_OBSERVER_H
#define LUPPP_TIME_OBSERVER_H

#include "../config.hxx"

class TimeObserver
{
  public:
    /// registers with TimeManager
    TimeObserver();
    virtual ~TimeObserver(){};
    
    virtual void setFpb(int fpb){};
    
    
    //virtual void bar(){LUPPP_WARN("bar() int not overriden");}
    
    virtual void bar(int framesInNframes){};
    
    virtual void beat(){};
    
    virtual void tapTempo(bool b){};
    
};

#endif // LUPPP_TIME_OBSERVER_H
