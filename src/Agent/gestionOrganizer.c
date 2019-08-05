#include "gestionOrganizer.h"

#define MAXDATASIZE 5000
#define ASK_WATCH		3
#define ASK_ORGANIZE	4
#define TYPE_MEAN		1

int initToOrganize(Agent * agMyself, char * nomToOrganize, char * nomToWatch, int nbPatternToOrganize)
{
	//We must find the pattern
	//First, we must look if it's one of ours
	slot* sl;
	int i=0;
	do
	{
		sl = vector_get(&(agMyself->listSlot),i);
		i++;
	}while(sl->type != SLOT_FREE && i < vector_total(&(agMyself->listSlot)));

	if(sl->type != SLOT_FREE && i == vector_total(&(agMyself->listSlot)))
	{
		return NO_SLOT_AVAILABLE;
	}

	sl->type = ASK_ORGANIZE;	
	sl->nomAgent = malloc(strlen(agMyself->name));
	sl->nbPattern = nbPatternToOrganize;
	strcpy(sl->nomAgent,agMyself->name);

	vector_add(&(sl->listToContact),nomToWatch);
	vector_add(&(sl->listToContact),nomToOrganize);
	if(strcmp(agMyself->name, nomToOrganize)==0)
	{
		//We have to watch ourself
		pattern* pat = vector_get(&(agMyself->listPattern),nbPatternToOrganize);
		
		for(int j=0;j<vector_total(&(pat->listFootprints));j++)
		{
			footprint* ftp = vector_get(&(pat->listFootprints),j);
			footprint* copy = malloc(sizeof(*ftp));
			copy->type = ftp->type;
			copy->validFrequency = ftp->validFrequency;
			copy->value = ftp->value;
			gettimeofday(&(copy->start), NULL);
			copy->processedValue = calloc(1,sizeof(int));
			vector_add(&(sl->listEmpreinte),copy);
		}
		sl->state = READY;
	}else
	{
		sl->state = NEED_INFORMATION;
		mxml_node_t *xml;
		mxml_node_t *data;

		char result[MAXDATASIZE];
		xml = mxmlNewXML("1.0");
		data = mxmlNewElement(xml, "toTransfer");
		mxmlElementSetAttr(data,"receiver",nomToOrganize);
		mxmlElementSetAttr(data,"type","10");
		char _cNbSlot[3];
		sprintf(_cNbSlot,"%d",nbPatternToOrganize);
		mxmlElementSetAttr(data,"nbPattern",_cNbSlot);
		
		mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
		client_TCP_envoi_message(agMyself->name,2,result);
		return INFORMATION_NEEDED;
	}

	printf("Slot : %d toOrganize : %s \n",i-1,nomToOrganize);
	return OK;
}

void updateOrganizer(slot* sl, Agent* ag)
{
	struct timeval end;
	gettimeofday(&end, NULL);
	for(int j=0;j<vector_total(&(sl->listEmpreinte));j++)
	{
		footprint* ptr = vector_get(&(sl->listEmpreinte),j);
		if((end.tv_sec * 1000000 + end.tv_usec) - (ptr->start.tv_sec * 1000000 + ptr->start.tv_usec) > 1000*ptr->validFrequency)
		{
			gettimeofday(&(ptr->start), NULL);
			char* nomWatcher = vector_get(&(sl->listToContact),0);
			if(strcmp(nomWatcher,sl->nomAgent)==0)
			{
				//Watcher = ourself
				//We must find the appropriate slot
				slot* slWatcher;
				int find = 0;
				for(int i=0;i<vector_total(&(ag->listSlot));i++)
				{
					slWatcher = vector_get(&(ag->listSlot),i);
					if(slWatcher->type == ASK_WATCH && slWatcher->nbPattern == sl->nbPattern)
					{
						if(strcmp(vector_get(&(slWatcher->listToContact),0),vector_get(&(sl->listToContact),1))==0)
						{
							//Gotcha
							find = 1;							
							break;
						}
					}
				}
				if(find == 1 && ptr->type == TYPE_MEAN)
				{
					footprint* ptrWatch = vector_get(&(slWatcher->listEmpreinte),j);
					ptr->processedValue[0] = ptrWatch->processedValue[ptrWatch->validFrequency/ptrWatch->samplingFrequency];
					printf("Dans l'organizer : %d %d\n",ptr->processedValue[0],ptr->value);
				}

			}else
			{

			}
		}
	}	
}