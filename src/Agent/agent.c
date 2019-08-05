#include "agent.h"

#define MAXDATASIZE 						5000
#define SLOT_FREE							1
#define ASK_WATCH							3
#define ASK_ORGANIZE						4
#define NEED_DATA_PATTERN_FOR_WATCHING		5
#define ANSWER_DATA_PATTERN_FOR_WATCHING	6
#define NEED_DATA_MONITOR_FOR_WATCHING		7
#define ANSWER_DATA_MONITOR_FOR_WATCHING	8
#define LAUNCH								9
#define NEED_DATA_PATTERN_FOR_ORGANIZER		10
#define ANSWER_DATA_PATTERN_FOR_ORGANIZER	11

static int state = 0;
static int isInit = 0;
char* PF_name;
pthread_t threadSlot;

void agent_init(char* nom, Agent* ag)
{
	ag->name = malloc(strlen(nom));
	strcpy(ag->name,nom);
	vector_init(&(ag->tabMessage));
	vector_init(&(ag->listPattern));
	vector_init(&(ag->listSlot));
	pthread_cond_init(&(ag->condAgent), NULL);
	pthread_mutex_init(&(ag->lockAgent), NULL);

	for(int i=0;i<ag->nbSlot;i++)
	{
		slot* sl;
		sl = malloc(sizeof(*sl));
		sl->type = SLOT_FREE;
		
		vector_init(&(sl->listToContact)) ;
		vector_init(&(sl->listEmpreinte)) ;
		vector_add(&(ag->listSlot),sl);
	}
}

void agent_run(Agent* ag)
{
	int rt;
	int totalMessage;
	while(1)
	{
		switch(state)
		{
			case 0:
			totalMessage = vector_total(&(ag->tabMessage));
			for(int i=totalMessage-1;i>=0;i--)
			{
				_message* message = vector_get(&(ag->tabMessage),i);
				
				//printf("Ici %d %s\n",strlen(message->content),message->content);
				
				if(isInit == 0)
				{
					if(message->type == INIT)
					{
							//Agent pas initialisé, on ne connaît pas notre nom unique.
						free(ag->name);

						ag->name = malloc(strlen(vector_get(&(message->listArg),0)));
						strcpy(ag->name,vector_get(&(message->listArg),0));

						PF_name = malloc(strlen(vector_get(&(message->listArg),1)));
						strcpy(PF_name,vector_get(&(message->listArg),1));

						mxml_node_t *xml;
						mxml_node_t *data;

						char result[MAXDATASIZE];
						xml = mxmlNewXML("1.0");
						data = mxmlNewElement(xml, "toTransfer");
						mxmlElementSetAttr(data,"receiver",PF_name);
						mxmlElementSetAttr(data,"type","2");
						char _cNbSlot[3];
						sprintf(_cNbSlot,"%d",ag->nbSlot);
						mxmlElementSetAttr(data,"nbSlot",_cNbSlot);
						char _cNbPattern[3];
						sprintf(_cNbPattern,"%d",vector_total(&(ag->listPattern)));
						mxmlElementSetAttr(data,"nbPattern",_cNbPattern);
						mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
						client_TCP_envoi_message(ag->name,2,result);
						isInit = 1;
					}

				}else
				{
					agent_traitement_message(message, ag);
				}
				vector_delete(&(ag->tabMessage),i);
				
				
			}
			state = 1;
			break;
			case 1:
			pthread_mutex_lock(&(ag->lockAgent));
			rt = pthread_cond_wait(&(ag->condAgent), &(ag->lockAgent));
			if(rt!=0)
			{
				printf("ERREUR : %d\n",rt);
			}else
			{
				state=0;
			}
			pthread_mutex_unlock(&(ag->lockAgent));

			break;
		}    	
	}
}

void agent_traitement_message(_message* mess, Agent* ag)
{
	char result[MAXDATASIZE];
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *dataBis;
	pattern* pat;
	switch(mess->type)
	{			
		case LAUNCH:
		pthread_create(&(threadSlot),NULL,gestionSlot,ag);

		break;
		case ASK_WATCH:
		initToWatch(ag, vector_get(&(mess->listArg),0), atoi(vector_get(&(mess->listArg),1)));
		break;
		case ASK_ORGANIZE:
		initToOrganize(ag, vector_get(&(mess->listArg),0), vector_get(&(mess->listArg),1), atoi(vector_get(&(mess->listArg),2)));
		break;
		case NEED_DATA_PATTERN_FOR_WATCHING:

		xml = mxmlNewXML("1.0");
		data = mxmlNewElement(xml, "toTransfer");
		mxmlElementSetAttr(data,"receiver",vector_get(&(mess->listArg),1));
		mxmlElementSetAttr(data,"type","6");
		mxmlElementSetAttr(data,"nbPattern",vector_get(&(mess->listArg),0));

		pat = vector_get(&(ag->listPattern),atoi(vector_get(&(mess->listArg),0)));
		for(int j=0;j<vector_total(&(pat->listFootprints));j++)
		{
			footprint* ftp = vector_get(&(pat->listFootprints),j);
			dataBis = mxmlNewElement(data, "footprint");
			char _cTarget[3];
			sprintf(_cTarget,"%d",ftp->target);
			mxmlElementSetAttr(dataBis,"target",_cTarget);
			char _cTtype[3];
			sprintf(_cTtype,"%d",ftp->type);
			mxmlElementSetAttr(dataBis,"type",_cTtype);
			char _cFS[3];
			sprintf(_cFS,"%d",ftp->samplingFrequency);
			mxmlElementSetAttr(dataBis,"samplingFrequency",_cFS);
			char _cFV[3];
			sprintf(_cFV,"%d",ftp->validFrequency);
			mxmlElementSetAttr(dataBis,"validFrequency",_cFV);
		}

		mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
		client_TCP_envoi_message(ag->name,2,result);
		break;



		case ANSWER_DATA_PATTERN_FOR_WATCHING:
		{
			for(int j=0;j<vector_total(&(ag->listSlot));j++)
			{
				slot* sl = vector_get(&(ag->listSlot),j);
				if(sl->type == ASK_WATCH)
				{
					if(sl->state == NEED_INFORMATION)
					{
						if(sl->nbPattern == atoi(vector_get(&(mess->listArg),0))
							&& strcmp(vector_get(&(sl->listToContact),0),vector_get(&(mess->listArg),1))==0)
						{
							for(int k = 0;k<(vector_total(&(mess->listArg))-2) /4;k++)
							{
								footprint* ftp = malloc(sizeof(*ftp));
								ftp->indice = 0;
								gettimeofday(&(ftp->start), NULL);
								ftp->validFrequency = atoi(vector_get(&(mess->listArg),2+4*k + 3));
								ftp->samplingFrequency = atoi(vector_get(&(mess->listArg),2+4*k + 2));
								ftp->target = atoi(vector_get(&(mess->listArg),2+4*k));
								ftp->type = atoi(vector_get(&(mess->listArg),2+4*k +1));
								ftp->processedValue = calloc((ftp->validFrequency/ftp->samplingFrequency) +2,sizeof(int));
								vector_add(&(sl->listEmpreinte),ftp);

							}
							sl->state = READY;
							break;
							
						}

					}
				}
			}
		}

		case NEED_DATA_MONITOR_FOR_WATCHING:
		//0 : NBPattern
		//1 : from
		
		xml = mxmlNewXML("1.0");
		data = mxmlNewElement(xml, "toTransfer");
		mxmlElementSetAttr(data,"receiver",vector_get(&(mess->listArg),1));
		mxmlElementSetAttr(data,"type","8");
		mxmlElementSetAttr(data,"nbPattern",vector_get(&(mess->listArg),0));
		pattern* ptr = vector_get(&(ag->listPattern),atoi(vector_get(&(mess->listArg),0)));
		footprint* ftp = vector_get(&(ptr->listFootprints),0);
		mxmlElementSetAttr(data,"value",ftp->observedValue);
		mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
		client_TCP_envoi_message(ag->name,2,result);
		break;
		case ANSWER_DATA_MONITOR_FOR_WATCHING:
		{
			for(int j=0;j<vector_total(&(ag->listSlot));j++)
			{
				slot* sl = vector_get(&(ag->listSlot),j);
				if(sl->type == ASK_WATCH)
				{
					if(sl->state == WAIT_INFORMATION)
					{
						if(sl->nbPattern == atoi(vector_get(&(mess->listArg),0))
							&& strcmp(vector_get(&(sl->listToContact),0),vector_get(&(mess->listArg),1))==0)
						{

							for(int l = 0;l<vector_total(&(sl->listEmpreinte));l++)
							{
								footprint* ptr1 = vector_get(&(sl->listEmpreinte),l);
								gettimeofday(&(ptr1->start), NULL);
								ptr1->observedValue = vector_get(&(mess->listArg),2);
								
								newDataForWatcher(ptr1);
							}
							sl->state = READY;
							break;
							
						}

					}
				}
			}
			break;
		}
		case NEED_DATA_PATTERN_FOR_ORGANIZER:
		xml = mxmlNewXML("1.0");
		data = mxmlNewElement(xml, "toTransfer");
		mxmlElementSetAttr(data,"receiver",vector_get(&(mess->listArg),1));
		mxmlElementSetAttr(data,"type","11");
		mxmlElementSetAttr(data,"nbPattern",vector_get(&(mess->listArg),0));

		pat = vector_get(&(ag->listPattern),atoi(vector_get(&(mess->listArg),0)));
		for(int j=0;j<vector_total(&(pat->listFootprints));j++)
		{
			footprint* ftp = vector_get(&(pat->listFootprints),j);
			dataBis = mxmlNewElement(data, "footprint");
			char _cTarget[3];
			sprintf(_cTarget,"%d",ftp->target);
			mxmlElementSetAttr(dataBis,"target",_cTarget);
			char _cTtype[3];
			sprintf(_cTtype,"%d",ftp->type);
			mxmlElementSetAttr(dataBis,"type",_cTtype);
			char _cFS[3];
			sprintf(_cFS,"%d",ftp->samplingFrequency);
			mxmlElementSetAttr(dataBis,"samplingFrequency",_cFS);
			char _cFV[3];
			sprintf(_cFV,"%d",ftp->validFrequency);
			mxmlElementSetAttr(dataBis,"validFrequency",_cFV);
			char _cValue[3];
			sprintf(_cValue,"%d",ftp->value);
			mxmlElementSetAttr(dataBis,"value",_cValue);
		}

		mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);
		client_TCP_envoi_message(ag->name,2,result);
		break;
		case ANSWER_DATA_PATTERN_FOR_ORGANIZER:
		
		for(int j=0;j<vector_total(&(ag->listSlot));j++)
		{
			slot* sl = vector_get(&(ag->listSlot),j);
			if(sl->type == ASK_ORGANIZE)
			{
				if(sl->state == NEED_INFORMATION)
				{
					if(sl->nbPattern == atoi(vector_get(&(mess->listArg),0))
						&& strcmp(vector_get(&(sl->listToContact),1),vector_get(&(mess->listArg),1))==0)
					{
						for(int k = 0;k<(vector_total(&(mess->listArg))-2) /5;k++)
						{
							footprint* ftp = malloc(sizeof(*ftp));
							ftp->indice = 0;
							gettimeofday(&(ftp->start), NULL);
							ftp->value = atoi(vector_get(&(mess->listArg),2+5*k + 4));
							ftp->validFrequency = atoi(vector_get(&(mess->listArg),2+5*k + 3));
							ftp->samplingFrequency = atoi(vector_get(&(mess->listArg),2+5*k + 2));
							ftp->target = atoi(vector_get(&(mess->listArg),2+5*k));
							ftp->type = atoi(vector_get(&(mess->listArg),2+5*k +1));
							ftp->processedValue = calloc(1,sizeof(int));
							vector_add(&(sl->listEmpreinte),ftp);

						}
						sl->state = READY;
						break;

					}

				}
			}
		}
		break;
	}
}

