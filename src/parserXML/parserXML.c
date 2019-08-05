#include "parserXML.h"


#define INIT 								1
#define TYPE_INIT_TO_PF						2
#define ASK_WATCH							3
#define ASK_ORGANIZE						4
#define NEED_DATA_PATTERN_FOR_WATCHING		5
#define ANSWER_DATA_PATTERN_FOR_WATCHING	6
#define NEED_DATA_MONITOR_FOR_WATCHING		7
#define ANSWER_DATA_MONITOR_FOR_WATCHING	8
#define LAUNCH								9
#define NEED_DATA_PATTERN_FOR_ORGANIZER		10
#define ANSWER_DATA_PATTERN_FOR_ORGANIZER	11


int openPattern(vector * listPattern, const char * restrict file)
{
	printf("Nom du fichier : %s \n",file);
	FILE *fp = NULL;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *nodeBis = NULL;
	

	if ( !file )
	{
		errno = EINVAL;
		return errno;
	}

	fp = fopen ( file, "r" );

	if ( !fp )
	{
		return -1;
	}
	tree = mxmlLoadFile ( NULL, fp, MXML_OPAQUE_CALLBACK );
	fclose ( fp );
	fp = NULL;

	//On doit pouvoir optimiser et enlever cette boucle... Mais pour l'instant, elle permet de compter le nombre d'action et de malloc comme il faut.
	for ( node = mxmlFindElement ( tree, tree, "pattern", NULL, NULL, MXML_DESCEND );
		node != NULL;
		node = mxmlFindElement ( node, tree, "footprint", NULL, NULL, MXML_DESCEND ) )
	{

		const char *name = mxmlGetElement ( node );
		if ( strcmp ( name, "footprint" ) == 0 )
		{
			footprint * _footPrint;
			_footPrint = malloc(sizeof(*_footPrint));
			pattern * pat = vector_get(listPattern,vector_total(listPattern)-1);
			const char* target = mxmlElementGetAttr ( node, "target" );
			_footPrint->target = atoi(target);
			const char* type = mxmlElementGetAttr ( node, "type" );
			_footPrint->type = atoi(type);
			const char* value = mxmlElementGetAttr ( node, "value" );
			_footPrint->value = atoi(value);
			const char* validFrequency = mxmlElementGetAttr ( node, "ValidFrequency" );
			_footPrint->validFrequency = atoi(validFrequency);
			const char* samplingFrequency = mxmlElementGetAttr ( node, "SamplingFrequency" );
			_footPrint->samplingFrequency = atoi(samplingFrequency);
			vector_add(&(pat->listFootprints),_footPrint);
		}else if(strcmp ( name, "pattern" ) == 0)
		{
			pattern* pat;
			pat = malloc(sizeof(*pat));
			vector_init(&(pat->listFootprints));
			vector_add(listPattern,pat);
		}
	}
	return 0;
}


int openMessage(vector * listMessage, char * message)
{
	int nbMessage = 0;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *nodeBis = NULL;
	mxml_node_t *nodeTer = NULL;
	char* content;
	tree = mxmlLoadString(NULL, message, MXML_OPAQUE_CALLBACK);
	if(tree == NULL)
	{
		return 0;
	}
	node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
	if(node != NULL)
	{
		for ( node = mxmlFindElement ( tree, tree, "publish", NULL, NULL, MXML_DESCEND );
			node != NULL;
			node = mxmlFindElement ( node, tree, "publish", NULL, NULL, MXML_DESCEND ) )
		{
			nodeBis = mxmlFindElement ( node, node, "content", NULL, NULL, MXML_DESCEND );
			if(nodeBis != NULL)
			{
				_message* mess;
				mess = malloc(sizeof(*(mess)));
				vector_init(&(mess->listArg));
				mess->emitterName = malloc(strlen("Serveur"));
				strcpy(mess->emitterName,"Serveur");	
				char * type = mxmlElementGetAttr ( nodeBis, "type" );
				if(type != NULL)
				{
					mess->type = atoi(type);
					switch(mess->type)
					{
						case LAUNCH:
						vector_add(&(mess->listArg),"Launch");
						break;
						case ASK_WATCH:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "toWatch" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "patternNumber" ));
						break;
						case ASK_ORGANIZE:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "toOrganize" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "isWatching" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "patternNumber" ));
						break;
						case NEED_DATA_PATTERN_FOR_WATCHING:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));
						break;
						case ANSWER_DATA_PATTERN_FOR_WATCHING:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));

						for ( nodeTer = mxmlFindElement ( nodeBis, nodeBis, "footprint", NULL, NULL, MXML_DESCEND );
							nodeTer != NULL;
							nodeTer = mxmlFindElement ( nodeTer, nodeBis, "footprint", NULL, NULL, MXML_DESCEND ) )
						{
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "target" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "type" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "samplingFrequency" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "validFrequency" ));
						}

						break;
						case NEED_DATA_MONITOR_FOR_WATCHING:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));
						break;
						case ANSWER_DATA_MONITOR_FOR_WATCHING:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "value" ));
						break;
						case NEED_DATA_PATTERN_FOR_ORGANIZER:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));
						break;
						case ANSWER_DATA_PATTERN_FOR_ORGANIZER:
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "nbPattern" ));
						vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeBis, "from" ));
						
						for ( nodeTer = mxmlFindElement ( nodeBis, nodeBis, "footprint", NULL, NULL, MXML_DESCEND );
							nodeTer != NULL;
							nodeTer = mxmlFindElement ( nodeTer, nodeBis, "footprint", NULL, NULL, MXML_DESCEND ) )
						{
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "target" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "type" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "samplingFrequency" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "validFrequency" ));
							vector_add(&(mess->listArg),mxmlElementGetAttr ( nodeTer, "value" ));
						}
						
						break;

					}
					vector_add(listMessage,mess);
					nbMessage++;					
				}
			}
		}
	}else
	{
		for ( node = mxmlFindElement ( tree, tree, "message", NULL, NULL, MXML_DESCEND );
			node != NULL;
			node = mxmlFindElement ( node, tree, "message", NULL, NULL, MXML_DESCEND ) )
		{
			_message* mess;
			mess = malloc(sizeof(*(mess)));
			vector_init(&(mess->listArg));
			mess->emitterName = malloc(strlen(mxmlElementGetAttr ( node, "name" )));
			strcpy(mess->emitterName,mxmlElementGetAttr ( node, "name" ));		
			mess->type = atoi(mxmlElementGetAttr ( node, "type" ));
			switch(mess->type)
			{
				case INIT:
				vector_add(&(mess->listArg),mxmlElementGetAttr ( node, "privateName" ));
				vector_add(&(mess->listArg),mxmlElementGetAttr ( node, "PFName" ));
				break;
				
			}
			vector_add(listMessage,mess);
			nbMessage++;
		}
	}
	
	return nbMessage;
}