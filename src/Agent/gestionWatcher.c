#include "gestionWatcher.h"
#include "../mxml/mxml.h"

#define MAXDATASIZE 5000
#define ASK_WATCH			3
#define TYPE_MEAN			1

int initToWatch(Agent * agMyself, char * nomToWatch, int nbPatternToWatch)
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

	sl->type = ASK_WATCH;	
	sl->nomAgent = malloc(strlen(agMyself->name));
	sl->nbPattern = nbPatternToWatch;

	strcpy(sl->nomAgent,agMyself->name);
	vector_add(&(sl->listToContact),nomToWatch);
	if(strcmp(agMyself->name, nomToWatch)==0)
	{
		//We have to watch ourself
		pattern* pat = vector_get(&(agMyself->listPattern),nbPatternToWatch);
		for(int j=0;j<vector_total(&(pat->listFootprints));j++)
		{
			footprint* ftp = vector_get(&(pat->listFootprints),j);
			ftp->indice = 0;
			gettimeofday(&(ftp->start), NULL);
			ftp->processedValue = calloc((ftp->validFrequency/ftp->samplingFrequency) +2,sizeof(int));
			vector_add(&(sl->listEmpreinte),ftp);
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
		mxmlElementSetAttr(data,"receiver",nomToWatch);
		mxmlElementSetAttr(data,"type","5");
		char _cNbSlot[3];
		sprintf(_cNbSlot,"%d",nbPatternToWatch);
		mxmlElementSetAttr(data,"nbPattern",_cNbSlot);
		
		mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
		client_TCP_envoi_message(agMyself->name,2,result);
		return INFORMATION_NEEDED;
	}
	return OK;
}

void updateWatcher(slot* sl)
{
	struct timeval end;
	gettimeofday(&end, NULL);
	if(strcmp(sl->nomAgent,vector_get(&(sl->listToContact),0)) == 0)
	{
		for(int j=0;j<vector_total(&(sl->listEmpreinte));j++)
		{
			footprint* ptr = vector_get(&(sl->listEmpreinte),j);
			if((end.tv_sec * 1000000 + end.tv_usec) - (ptr->start.tv_sec * 1000000 + ptr->start.tv_usec) > 1000*ptr->samplingFrequency)
			{
				gettimeofday(&(ptr->start), NULL);
				newDataForWatcher(ptr);
			}
		}	
	}else
	{
		for(int j=0;j<vector_total(&(sl->listEmpreinte));j++)
		{
			footprint* ptr = vector_get(&(sl->listEmpreinte),j);
			if((end.tv_sec * 1000000 + end.tv_usec) - (ptr->start.tv_sec * 1000000 + ptr->start.tv_usec) > 1000*ptr->samplingFrequency)
			{
				mxml_node_t *xml;
				mxml_node_t *data;
				gettimeofday(&(ptr->start), NULL);

				char result[MAXDATASIZE];
				xml = mxmlNewXML("1.0");
				data = mxmlNewElement(xml, "toTransfer");
				mxmlElementSetAttr(data,"receiver",vector_get(&(sl->listToContact),0));
				mxmlElementSetAttr(data,"type","7");
				char _cNbSlot[3];
				sprintf(_cNbSlot,"%d",sl->nbPattern);
				mxmlElementSetAttr(data,"nbPattern",_cNbSlot);

				mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
				client_TCP_envoi_message(sl->nomAgent,2,result);
				sl->state = WAIT_INFORMATION;
			}
		}		
	}	
}

void newDataForWatcher(footprint* ftp)
{
	ftp->processedValue[ftp->indice] = atoi(ftp->observedValue);
	ftp->indice++;
	ftp->indice = ftp->indice % (ftp->validFrequency/ftp->samplingFrequency);
	if(ftp->type == TYPE_MEAN)
	{
		int sum = 0;
		for(int k=0;k<ftp->validFrequency/ftp->samplingFrequency;k++)
		{
			sum += ftp->processedValue[k];
		}
		ftp->processedValue[ftp->validFrequency/ftp->samplingFrequency] = sum / ((ftp->validFrequency)/(ftp->samplingFrequency));
	}
}