#include "gestionSlot.h"
#include <sys/time.h>
#include <time.h>
#define ASK_WATCH			3
#define ASK_ORGANIZE		4




void gestionSlot(Agent* ag)
{	
	while(1)
	{
		usleep(10000);
		for(int i=0;i<vector_total(&(ag->listSlot));i++)
		{
			slot* sl = vector_get(&(ag->listSlot),i);
			if(sl->state == READY)
			{
				if(sl->type == ASK_WATCH)
				{				
					updateWatcher(sl);			
				}else if(sl->type == ASK_ORGANIZE)
				{				
					updateOrganizer(sl,ag);			
				}
			}
			
		}
	}	
}