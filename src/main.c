// file auto generated

#include <stdlib.h>
#include "./signalHandler/signalHandler.h"
#include "./log/log.h"
#include "./freeOnExit/freeOnExit.h"
#include <stdint.h>
#include "./config/config_arg.h"
#include "./config/config_file.h"
#include "./parserXML/parserXML.h"
#include "./Client_TCP/clientTCP.h"
#include "./struct/vector.h"
#include "./struct/slot.h"
#include "./Agent/agent.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void GetCPULoad(Agent* ag) 
{
	int FileHandler;
	char FileBuffer[1024];
	float load;
	char* array;

	array = malloc(4);
	while(1)
	{
		FileHandler = open("/proc/loadavg", O_RDONLY);
		if(FileHandler < 0) 
		{

		}
		read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
		sscanf(FileBuffer, "%f", &load);
		close(FileHandler);
		sprintf(array, "%d", (int)(load * 100));
		for(int i=0;i<vector_total(&(ag->listPattern));i++)
		{
			pattern* pat = vector_get(&(ag->listPattern),i);
			for(int j=0;j<vector_total(&(pat->listFootprints));j++)
			{
				footprint* ftp = vector_get(&(pat->listFootprints),j);
				if(ftp->target == TARGET_CPU_ACTIVITY)
				{
					ftp->observedValue = array;
				}
			}
		}
		
		usleep(10000);
	}


}


// INIT_FUNCTION
void functionExit ( void * arg )
{

	printf ( "%s\n", ( char * )arg );

	exit ( 0 );
}


int main ( int argc, char ** argv )
{
	// INIT_VAR
	signalHandling signal =
	{
		.flag =
		{
			.Int = 1,
			.Quit = 1,
			.Term = 1
		},
		.Int =
		{
			.func = functionExit,
			.arg = "Int case\n"
		},
		.Quit =
		{
			.func = functionExit,
			.arg = "Quit case\n"
		},
		.Term =
		{
			.func = functionExit,
			.arg = "Term case\n"
		}
	};

	struct
	{
		#ifdef __LOG_H__
		uint8_t help:1,
		quiet:1,
		verbose:1,
			#ifndef RELEASE_MODE
				color:1, // only available for debug mode
				debug:1, // only available for debug mode
			#else
				unused:2,
			#endif
				term:1,
				file:1;
		#else
				uint8_t help:1,
			unused:6; // to have same allignement with and without debug/color/quiet flags
		#endif
		}
		flags = { 0 };
	#ifdef __LOG_H__
		char logFileName[ 512 ] = "";
	#endif

		char address[ 16 ]="127.0.0.1";
		uint16_t port = 12345;
		uint16_t nbSlot = 5;
		char agentName[ 256 ]="Bob" ;
		Agent ag;

		pthread_t threadCPULoad;

		param_el param[] =
		{
			{ "--help", "-h", 0x01, cT ( bool ), &flags, "help" },
		#ifdef __LOG_H__
			{ "--quiet", "-q", 0x02, cT ( bool ), &flags, "quiet" },
			{ "--verbose", "-v", 0x04, cT ( bool ), &flags, "verbose" },
			#ifndef RELEASE_MODE
			{ "--color", "-c", 0x08, cT ( bool ), &flags, "color" },
			{ "--debug", "-d", 0x10, cT ( bool ), &flags, "debug" },
			#endif
			{ "--term", "-lT", 0x20, cT ( bool ), &flags, "log on term" },
			{ "--file", "-lF", 0x40, cT ( bool ), &flags, "log in file" },
			{ "--logFileName", "-lFN", 1, cT ( str ), logFileName, "log file name" },
		#endif
			{ "--name", "-n", 1, cT ( str ), agentName, "Agent's name" },
			{ "--address", "-a", 1, cT ( str ), address, "Server's address" },
			{ "--port", "-p", 1, cT ( uint16_t ), &port, "Server's port" },
			{ "--nbSlot", "-s", 1, cT ( uint16_t ), &nbSlot, "Nombre de slots" },
			{ NULL }
		};

	// INIT_CORE
	// INIT_SIGNALHANDLER
		signalHandlerInit ( &signal );
	// END_SIGNALHANDLER

	// INIT_FREEONEXIT
		if ( initFreeOnExit ( ) )
	{ // failure case
	}
	// END_FREEONEXIT
	
	// INIT_CONFIG
	if ( readParamArgs ( argc, argv, param ) )
	{ // failure case
	}
	// END_CONFIG
	// INIT_LOG
	#ifdef __CONFIG_DATA_H__
	logSetVerbose ( flags.verbose );
		#ifndef RELEASE_MODE
	logSetDebug ( flags.debug );
	logSetColor ( flags.color );
		#endif
	logSetQuiet ( flags.quiet );
	logSetOutput ( flags.term | !flags.file, flags.file );
	logSetFileName ( logFileName );
	
	#endif
	// END_LOG
	printf ( "main start mode: %s\n", (flags.debug)?"debug":(flags.verbose)?"verbose":"normal" );
	printf ( "print: %s\n", (flags.file)?"file":"terminal" );
	logVerbose ( "Verbose Activated\n" );
	logDebug ( "Debug Activated\n" );


	ag.nbSlot = nbSlot;
	agent_init(agentName, &ag);
	openPattern(&(ag.listPattern), "bin/config.xml");
	printf("nbPattern : %d\n",vector_total(&(ag.listPattern)));
	for(int i=0;i<vector_total(&(ag.listPattern));i++)
	{
		pattern* pat = vector_get(&(ag.listPattern),i);
		printf("nbEmpreinte du pattern %d : %d\n",i,vector_total(&(pat->listFootprints)));
		for(int j=0;j<vector_total(&(pat->listFootprints));j++)
		{
			footprint* ftp = vector_get(&(pat->listFootprints),j);
			printf("\n\nFootprint numéro %d : \n",j);
			printf("Target : %d\n",ftp->target);
			printf("Type : %d\n",ftp->type);
			printf("Value : %d\n",ftp->value);
			printf("samplingFrequency : %d\n",ftp->samplingFrequency);
			printf("validFrequency : %d\n",ftp->validFrequency);
		}
	}
	if ( client_TCP_init_connec ( address, port, &ag ) )
	{
		printf ( "no server available\n" );
		return(  0 );
	}
	client_TCP_envoi_message(agentName,1,"2");
	// END_CORE
	// END_PROGRAM
	pthread_create(&(threadCPULoad),NULL,GetCPULoad,&ag);

	agent_run(&ag);
	return ( 0 );
}

