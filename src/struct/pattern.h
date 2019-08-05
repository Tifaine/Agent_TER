#ifndef __PATTERN_H__
#define __PATTERN_H__
#include "vector.h"


typedef struct footprint
{
	int type;
	int target;
	int samplingFrequency;
	int value;
	int validFrequency;
	char* observedValue;
	int* processedValue;
	int indice;
	struct timeval start;

}footprint;

typedef struct pattern
{
	vector listFootprints; 
}pattern;

#endif //__PATTERN_H__