
#ifndef LUPPP_TIME_OBSERVER_H
#define LUPPP_TIME_OBSERVER_H

class TimeObserver
{
  public:
    /// registers with TimeManager
    TimeObserver();
    
    virtual void setFpb(int fpb){};
    
    virtual void bar(){};
    virtual void beat(){};
    
};

#endif // LUPPP_TIME_OBSERVER_H
