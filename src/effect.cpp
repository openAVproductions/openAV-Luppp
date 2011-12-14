
#include "effect.hpp"

#include "top.hpp"

int Effect::privateID = 0;

Effect::Effect(Top* t)
{
  ID = privateID++;
  t->state.addEffectState();
}
