
#ifndef LUPPP_CONTROLLER
#define LUPPP_CONTROLLER

class Top;

class Controller
{
  public:
    Controller(Top*);
    
    void setTrackDevice(int,int);
    int getTrack();
    int getDevice();
  
  private:
    Top* top;
    
    int track;
    int device;
};

#endif
