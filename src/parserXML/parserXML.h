#ifndef __PARSERXML_H__
#define __PARSERXML_H__

#include "../mxml/mxml.h"
#include "../struct/pattern.h"
#include "../struct/vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



typedef struct _message
{
	char* emitterName;
	int type;
	vector listArg;
}_message;

int openPattern(vector * listPattern, const char * restrict file);
int openMessage(vector * listMessage, char * message);

#endif //__PARSERXML_H__