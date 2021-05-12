#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//some definitions
#define FALSE 0
#define TRUE 1
#define ADDR long long
#define BOOL char
#define BYTE 8
typedef struct _MEMACCESS {
	ADDR addr;
	BOOL is_read;
} MEMACCESS;

typedef enum _RPL { LRU = 0, RAND = 1 } RPL;

typedef struct _node {
	BOOL V;
	ADDR tag;
	int data[32];
	short counter;
}node;

//misc. function
FILE* fp = 0;
char trace_file[100] = "memtrace.trc";
BOOL read_new_memaccess(MEMACCESS*);  //read new memory access from the memory trace file (already implemented)

int size_tag;
int size_index;
int size_offset;

//configure the cache
node** init_cache(int cache_size, int block_size, int assoc, RPL repl_policy) {
	node **table;
	int i;
	table = (node**)malloc(sizeof(node*)*assoc);
	for (i = 0; i < assoc; i++) {
		table[i] = calloc((int)(cache_size / block_size / assoc), sizeof(node));
	}
	size_offset = (int)(log((double)block_size) / log(2));
	size_index = (int)(log((double)(cache_size / block_size / assoc)) / log(2));
	size_tag = sizeof(ADDR) * BYTE - size_offset - size_index;
	return table;
}

//check if the memory access hits on the cache
ADDR isHit(ADDR addr, int assoc, node **table) {
	ADDR offset = addr & (ADDR)(pow(2, (double)size_offset) - 1);
	ADDR index = (addr & (ADDR)(pow(2, (double)(size_index + size_offset)) - 1)) >> size_offset;
	ADDR tag = addr >> (size_offset + size_index);
	int i, tmp;

	for (i = 0; i < assoc; i++) {
		if (table[i][index].V && table[i][index].tag == tag) {
			for (tmp = 0; tmp < assoc; tmp++)
				if (tmp != i && table[i][index].counter < table[tmp][index].counter)
					table[tmp][index].counter--;
			table[i][index].counter = assoc - 1;
			return TRUE; //return data;
		}
	}
	return FALSE;
}

//insert a cache block for a memory access
ADDR insert_to_cache(ADDR addr, node** table, int assoc, RPL type) {
	ADDR offset = addr & (ADDR)(pow(2, (double)size_offset) - 1);
	ADDR index = (addr & (ADDR)(pow(2, (double)(size_index + size_offset)) - 1)) >> size_offset;
	ADDR tag = addr >> (size_offset + size_index);
	static int pre_val = 0;
	int i;
	int j = 0, min = 0;
	if (type == LRU) {
		for (i = 0; i < assoc; i++) {
			if (!table[i][index].V) {
				table[i][index].V = TRUE;
				table[i][index].tag = tag;
				//table[i][index].data[offset >> 2]; //data table[i][index].data[offset>>2] |= data <<< (offset& 3);

				for (j = 0; j < assoc; j++)
					if (j != i && table[j][index].counter > table[i][index].counter)
						table[j][index].counter--;
				table[i][index].counter = assoc - 1;
				return (_int64)NULL;
			}
			if (table[min][index].counter > table[i][index].counter)
				min = i;
		}

		table[min][index].tag = tag;

		for (i = 0; i < assoc; i++)
			if (min != i && table[i][index].counter > table[min][index].counter)
				table[i][index].counter--;
		table[min][index].counter = assoc - 1;
		return (_int64)NULL;
	}

	else if (type == RAND) {
		for (i = 0; i < assoc; i++) {
			if (!table[i][index].V) {
				table[i][index].V = TRUE;
				table[i][index].tag = tag;
				//table[i][index].data[offset >> 2]; //data table[i][index].data[offset>>2] |= data <<< (offset& 3);
				return (_int64)NULL;
			}
		}

		i = rand() % assoc;
		if (i == pre_val)
			i = rand() % assoc;
		pre_val = i;
		table[i][index].V = TRUE;
		table[i][index].tag = tag;
		return (_int64)NULL;
	}
	return (_int64)NULL;
}

//print the simulation statistics

//main
int main(int argc, char*argv[])
{
	int i = 0, total = 0, hit = 0;
	int cache_size = 32768;
	int assoc = 8;
	int block_size = 32;
	RPL repl_policy = LRU;
	node** table;
	/*
	*  Read through command-line arguments for options.
	*/
	srand(time(NULL));
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 's')
				cache_size = atoi(argv[i + 1]);

			if (argv[i][1] == 'b')
				block_size = atoi(argv[i + 1]);

			if (argv[i][1] == 'a')
				assoc = atoi(argv[i + 1]);

			if (argv[i][1] == 'f')
				strcpy(trace_file, argv[i + 1]);

			if (argv[i][1] == 'r')
			{
				if (strcmp(argv[i + 1], "lru") == 0)
					repl_policy = LRU;
				else if (strcmp(argv[i + 1], "rand") == 0)
					repl_policy = RAND;
				else
				{
					printf("unsupported replacement policy:%s\n", argv[i + 1]);
					return -1;
				}
			}
		}
	}

	/*
	* main body of cache simulator
	*/

	table = init_cache(cache_size, block_size, assoc, repl_policy);   //configure the cache with the cache parameters specified in the input arguments

	while (1)
	{
		MEMACCESS new_access;

		BOOL success = read_new_memaccess(&new_access);  //read new memory access from the memory trace file

		if (success != TRUE)   //check the end of the trace file
			break;

		if (isHit(new_access.addr, assoc, table) == FALSE)   //check if the new memory access hit on the cache
		{
			//printf("miss\n");
			insert_to_cache(new_access.addr, table, assoc, repl_policy);  //if miss, insert a cache block for the memory access to the cache
		}

		else {
			hit++;
			//printf("hit\n");
		}

		total++;
		//printf("trace Number: %d\n", total);
	}
	printf("cache_size:      %d B\n", cache_size);
	printf("block_size:      %d B\n", block_size);
	printf("associativity:      %d\n", assoc);
	printf("replacement policy:   %s\n", (repl_policy) ? "Random" : "LRU");
	printf("cache access:      %d\n", total);
	printf("cache_hits:      %d\n", hit);
	printf("cache_misses:      %d\n", total - hit);
	printf("cache_miss_rate:   %lf%%\n", (double)(total - hit) / total * 100);
	// print statistics here
	getchar();
	return 0;
}

/*
* read a new memory access from the memory trace file
*/
BOOL read_new_memaccess(MEMACCESS* mem_access)
{
	ADDR access_addr;
	char access_type[20];
	/*
	* open the mem trace file
	*/

	if (fp == NULL)
	{
		fp = fopen(trace_file, "r");
		if (fp == NULL)
		{
			fprintf(stderr, "error opening file");
			exit(2);
		}
	}

	if (mem_access == NULL)
	{
		fprintf(stderr, "MEMACCESS pointer is null!");
		exit(2);
	}

	if (fscanf(fp, "%llx %s", &access_addr, &access_type) != EOF)
	{
		mem_access->addr = access_addr;
		if (strcmp(access_type, "RD") == 0)
			mem_access->is_read = TRUE;
		else
			mem_access->is_read = FALSE;

		return TRUE;
	}
	else
		return FALSE;
}