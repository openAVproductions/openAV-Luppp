
#include "g_widgetbase.hpp"

int WidgetBase::privateID = 0;

WidgetBase::WidgetBase()
{
  ID = privateID++;
}

int WidgetBase::getID()
{
  return ID;
}
