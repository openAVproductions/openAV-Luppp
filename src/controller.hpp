
#ifndef LUPPP_CONTROLLER
#define LUPPP_CONTROLLER

class Top;

class Controller
{
  public:
    Controller(Top*);
    
    int track;
    int device;
  private:
    Top* top;
};

#endif
