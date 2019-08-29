#ifndef __AGENT_H__
#define __AGENT_H__

#include <assert.h>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "../struct/vector.h"
#include "../struct/agent.h"
#include "../struct/slot.h"
#include "../parserXML/parserXML.h"
#include "gestionWatcher.h"
#include "gestionSlot.h"

#define INIT 1
#define TARGET_CPU_ACTIVITY 		1
#define TARGET_CURRENT_ACTIVITY 	2


void agent_init(char* nom, Agent* ag);
void agent_run(Agent* ag);
void agent_traitement_message(_message* mess, Agent* ag);

#endif //__AGENT_H__