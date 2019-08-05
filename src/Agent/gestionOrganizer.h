#ifndef __GESTION_ORGANIZE_H__
#define __GESTION_ORGANIZE_H__

#include "agent.h"
#include "../struct/slot.h"
#include "../struct/pattern.h"


int initToOrganize(Agent * agMyself,char * nomToOrganize, char * nomToWatch, int nbPatternToOrganize);
void updateOrganizer(slot* sl, Agent* ag);

#endif //__GESTION_ORGANIZE_H__
