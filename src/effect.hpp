
#ifndef LUPPP_EFFECT
#define LUPPP_EFFECT

#include <iostream>
#include <vector>

class Top;

class Effect
{
  public:
    Effect();
    virtual void process(int, float*) = 0;
  
  protected:
    
};

#endif

