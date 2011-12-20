
#ifndef LUPPP_WIDGETBASE
#define LUPPP_WIDGETBASE

class WidgetBase
{
  public:
    WidgetBase();
    
    // used to get the EffectState* from the vector
    int  getID();
  
  private:
    int ID;
    static int privateID;
};

#endif
