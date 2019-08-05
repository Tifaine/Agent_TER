#ifndef __GESTION_WATCHER_H__
#define __GESTION_WATCHER_H__

#include "agent.h"
#include "../struct/slot.h"
#include "../struct/pattern.h"



int initToWatch(Agent * agMyself, char * nomToWatch, int nbPatternToWatch);
void updateWatcher(slot* sl);
void newDataForWatcher(footprint* ftp);

#endif //__GESTION_WATCHER_H__