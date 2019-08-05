#include "clientTCP.h"
#include "../freeOnExit/freeOnExit.h"

#include "../log/log.h"

#define MAXDATASIZE 5000
static int sockFileDescriptor;
static int isConnected = 0;

pthread_t threadClientTCP;

int client_TCP_init_connec(char* addr, int port, void* arg)
{
	
	struct sockaddr_in servaddr;
	sockFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(sockFileDescriptor == -1)
	{
		printf("Failed \n");
		return -1; 
	}
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(addr); 
	servaddr.sin_port = htons(port);
	if (connect(sockFileDescriptor, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
	{ 
		printf("connection with the server failed...\n"); 
		return -2; 
	}

	printf("connected to the server.\n"); 

	isConnected = 1;
	pthread_create(&(threadClientTCP),NULL,client_TCP_attente_message,arg);
	return 0;
}

int client_TCP_envoi_message(char* nom_emetteur, int type, char* message)
{

	mxml_node_t *xml;
	mxml_node_t *data;
	char _cType[2];
	char result[MAXDATASIZE];
	sprintf(_cType, "%d", type);

	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "message");
	mxmlElementSetAttr(data,"name",nom_emetteur);
   
    mxmlElementSetAttr(data,"type",_cType);
    if(type==1)
    {
    	mxmlElementSetAttr(data,"content",message);
    }else
    {
    	mxml_node_t *tree = NULL;
    	tree = mxmlLoadString(NULL, message, MXML_OPAQUE_CALLBACK);
    	mxmlAdd(data,MXML_ADD_AFTER,MXML_ADD_TO_PARENT,tree);
    }
   
	mxmlSaveString(xml , result, MAXDATASIZE, MXML_NO_CALLBACK);

	if(send(sockFileDescriptor, result, strlen(result), MSG_CONFIRM) == -1)
	{
		free ( result );
		perror("send");
		return (1);
	}

	//free(result);
	return 0;
}

void* client_TCP_attente_message( void* arg )
{
	int mustPreventAgent = 0;
	while( 1 )
	{
		if( isConnected == 1 )
		{
			char* buf = calloc( MAXDATASIZE, sizeof( char ) );
			if ( recv( sockFileDescriptor, buf, MAXDATASIZE, 0 ) > 0 )
			{
				int nbMess = openMessage(&((Agent*)arg)->tabMessage,buf);
				if ( nbMess > 0 )
				{
					pthread_mutex_lock((&((Agent*)arg)->lockAgent));
					pthread_cond_signal((&((Agent*)arg)->condAgent));
					pthread_mutex_unlock((&((Agent*)arg)->lockAgent));					
				}
			}
			else
			{
				free ( buf );
				return NULL;
			}    			
		}			
	}	
}

int clientKillAll ( void )
{
	pthread_cancel ( threadClientTCP );
}