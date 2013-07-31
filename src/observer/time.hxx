
#ifndef LUPPP_OBSERVER_H
#define LUPPP_OBSERVER_H

class TimeObserver
{
  public:
    virtual void setFpb(int fpb){};
    
    virtual void bar(){};
    virtual void beat(){};
    
};

#endif // LUPPP_OBSERVER_H
