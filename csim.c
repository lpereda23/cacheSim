/**
 * Leonardo Pereda
 * April 16, 2018
 * 101935650
 * Cache Lab Part A 
 */
#include <stdio.h>
#include <unistd.h>
#include "cachelab.h"
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

 /**
  * Declaration of functions in order to avoid implicit declararation errors.
 */
void usage();
void printSummary(int hits, int misses, int evictions);
int debug = 1;

/**
 * Declare all structs that will be used in the cache lab
 * They are all linked together with pointers
*/
typedef struct
{
	int used;
	int validBit;
	long long tag;
	long data;
}cacheLine;

typedef struct
{
	cacheLine *cLine;
}cacheSet;

typedef struct 
{
	cacheSet *cSet;
}Cache;

/**
 * More declarations of other functions
*/
void readFile(FILE *pfile, Cache c, int e, int s, int b);
void cSimu(Cache c, unsigned long address, int size, int e, int s, int b);
Cache organizeCache(int s, int E, int b);

/**
 * Here in the maain I use a switch in order to parse the command line args
 * as each arg is being checked it will be being saved in oder to use later on 
 * After parsing in I set the cache by allocating the memory for the entire cache with 
 * a helper function organizeCache. Then cache is passed into readFile in order to look 
 * at the trace file and in that function the simulation is being called. 
*/
int main(int argc, char **argv)
{
	FILE *pfile;
	Cache wholeCache;
    int option, s, E, b;
    while((option = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch(option)
        {
            case 'h':
              usage();
              break;
            case 'v':
              printf("verbose flag ON\n");
              break;
            case 's':
              s = atoi(optarg);
              break;
            case 'E':
              E = atoi(optarg);
              break;
            case 'b':
              b = atoi(optarg);
              break;
            case 't':
              pfile = fopen(optarg, "r");
              break;
            default:
              printf("Invalid argument.\n");
			  usage();
        }
    }
	wholeCache = organizeCache(s,E,b);
	if(debug) printf("About to go in to readFile function...\n");
	readFile(pfile, wholeCache, E, s, b);
	printSummary(0, 0, 0);
	return 0; 
}

/**
 * This function is the helper function to organize and create
 * the whole cache. This will simply make a cache with the 
 * given dimensions. It is an empty cache it will be filled up 
 * in another function. I have a helping debug which is off by
 * default.
*/
Cache organizeCache(int s, int E, int b)
{	
	Cache allcache;
	double base = 2;
	int lines = E;
	long S = pow(base, s);
	long B = pow(base, b);
	allcache.cSet = (cacheSet *)malloc(sizeof(cacheSet)*S);
	for (int i = 0; i<S; i++)
	{
		allcache.cSet[i].cLine = (cacheLine *)malloc(sizeof(cacheLine)*E);
		for(int j = 0; j < E; j++)
		{
			allcache.cSet[i].cLine[j].validBit = 0;
			allcache.cSet[i].cLine[j].tag = 0;
			allcache.cSet[i].cLine[j].data = 0;
			allcache.cSet[i].cLine[j].used = 0;
		}
	}
	if(debug == 1)
	{
		printf("\n");
		printf("**************\n");
		printf("organizeCache\n");
		printf("S = %ld\n",S);
		printf("E = %d\n",lines);
		printf("B = %ld\n",B);
		printf("**************\n");
		printf("\n");
	}
	return allcache;
}

/**
 * This function is being used as the second part of setting up 
 * the entire cache. Here an empty (already allocated cache is passed in)
 * Goes through the file and looks the operation and from there calls cSimu 
 * to simulate the function. 
 */
void readFile(FILE *pfile, Cache c, int e, int s, int b)
{
	char operation;
	long address;
	int size;

	if(debug) printf("In the readFile function\n");
	if(pfile == NULL)
	{
		perror("Error in opening file\n");
	}
	/** Here it's filling in the cache with the file input */
	while(fscanf(pfile, " %c %lx,%d", &operation, &address, &size) > 0)
	{
		switch(operation)
		{
			case 'M':
				cSimu(c, address, size, e, s, b);
				break;
			case 'L':
				cSimu(c, address, size, e, s, b);
				break;
			case 'S':
				cSimu(c, address, size, e, s, b);
				break;
			default:
				printf("I was used\n");
				
		}
	}
}
 /**
  * cSimu is a function to similate the entire cache is called in the readLine function
 */
void cSimu(Cache c, unsigned long address, int size, int e, int s, int b)
{
	short ocupado = 1;
	int hits = 0;
	int evictions = 0;
	unsigned long setAddress = (address << (64 - s - b)) >> (64 - s);
	unsigned long int localTag = address >> (s + b);
	cacheSet *currentSet = &c.cSet[setAddress];

	if(debug) printf("cSimu e = %d\n", e);
	for(int lns = 0; lns < e; lns++)
	{
		if(currentSet->cLine[lns].validBit && currentSet->cLine[lns].tag == localTag)
		{
			if(debug)printf("We have a hit!!!!!!\n");
			hits++;
		}
		if(!currentSet->cLine[lns].validBit)
		{
			if(debug)printf("Validbit check goes into else\n");
			ocupado = 0;
		}
	}
	if(ocupado)
	{
		if(debug)printf("We have to evict!!!\n");
		evictions++;
	}
}







/**
 * usage function for clarified instructions
*/
void usage()
{
    printf("Specific Error [to be displayed here]\n");
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options\n");
    printf("-h          Print this help message.\n");
    printf("-v          Optional verbose flag.\n");
    printf("-s <num>    Number of set index bits.\n");
    printf("-E <num>    Number of lines per set.\n");
    printf("-b <num>    Number of block offset bits.\n");
    printf("-t <file>   Trace file.\n");
}

