#ifndef __AGENT_STRUCT_H__
#define __AGENT_STRUCT_H__

#include <assert.h>
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "vector.h"

typedef struct Agent
{
	// Declaration of thread condition variable
	pthread_cond_t condAgent;

	// declaring mutex 
	pthread_mutex_t lockAgent;

	vector tabMessage;

	char* name;
	pthread_t threadSlot;
	int isSecurised;
	int nbSlot;

	vector listPattern;
	vector listSlot;

}Agent;



#endif //__AGENT_STRUCT_H__