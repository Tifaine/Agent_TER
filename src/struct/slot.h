#ifndef __SLOT_H__
#define __SLOT_H__
#include "vector.h"
#include "pattern.h"
#include <sys/time.h>
#include <time.h>
#define SLOT_FREE			1
#define NEED_INFORMATION	2
#define WAIT_INFORMATION	3
#define READY				4


#define NO_SLOT_AVAILABLE	-1
#define INFORMATION_NEEDED	2
#define OK					1
typedef struct slot
{
	int type;
	int state;
	char* nomAgent;
	int nbPattern;
	vector listToContact;
	vector listEmpreinte;
	
}slot;

#endif //__SLOT_H__