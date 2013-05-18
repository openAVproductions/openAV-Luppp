

#ifndef LUPPP_CONTROLLER_H
#define LUPPP_CONTROLLER_H

class Controller
{
  public:
    Controller();
    virtual ~Controller();
    
    virtual void mute(int t, bool b);
    virtual void clip(int t, bool b);
    
    virtual void record(int t, bool b);
    
    virtual void volume(int t, float f);
};

#endif // LUPPP_CONTROLLER_H

