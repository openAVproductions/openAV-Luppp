
#include "effect.hpp"

#include "top.hpp"

int Effect::privateID = 0;

Effect::Effect(Top* t, EffectType& et)
{
  ID = privateID++;
  t->state.addEffectState(ID);
  
  type = et;
}
