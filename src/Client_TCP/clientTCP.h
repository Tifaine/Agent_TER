/**
* 
*
* Copyright (c) 2019 STMicroelectronics - All Rights Reserved
* Copyright (c) 2019 CREN - Action de Recherche Tifaifai
*
* \file clientTCP.h
* \brief Client TCP permettant le lien avec un serveur TCP.
* \author Guyot Tifaine <tifaine.guyot@univ-lemans.fr> <tifaine.guyot@st.com>
* \version 0.1
* \date 03/06/2019
*
* Programme permettant de lancer un client TCP. Il va ainsi permettre à notre logiciel de communiquer avec un serveur.
*
*/

#ifndef CLIENT_TCP_H
#define CLIENT_TCP_H

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include "../struct/vector.h"
#include "../Agent/agent.h"
#include "../mxml/mxml.h"
#include <unistd.h>
#include <sys/time.h>

/*
	Se connecte à un serveur TCP à l'adresse passée en paramètre sur le port donné.
	return 0 si la connexion est effective, 
	-1 si la socket ne s'est pas créée,
	-2 si la connexion au server a échouée
*/
int client_TCP_init_connec(char* addr, int port, void* arg);

/*
	Envoie un message au serveur
*/
int client_TCP_envoi_message(char* nom_emetteur, int type, char* message);

/*
	Thread d'attente d'un message, notifie un mutex en cas de message, débloquant l'agent
*/
void* client_TCP_attente_message(void* arg);

#endif //CLIENT_TCP_H