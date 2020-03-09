// Chenyang Wang
// WUSTL key: chenyang.wang
// This lab is hard!!!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "cachelab.h"

void usage(char* process){//print out usage as required
	printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", process);
	printf("Options:\n");
	printf("  -h              Optional help flag that prints usage info\n");
	printf("  -v              Optional verbose flag that displays trace info\n");
	printf("  -s <s>          Number of set index bits\n");
    printf("  -E <E>          Associativity (number of lines per set)\n");
	printf("  -b <b>          Number of block bits\n");
	printf("  -t <tracefile>  Name of the valgrind trace to replay\n"); 
    printf("\nExamples:\n");
	printf("  linux> %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", process);
    printf("  linux> %s -v -s 4 -E 1 -b 4 -t traces/yi.trace\n", process);
}

typedef struct {
	int s,e,b,v;
	// s: set index bits
	// b: block offset bits
	// e: lines per set
	// v: verbose
} param_t;

typedef struct {
	int hits;
	int misses;
	int evictions;
	unsigned long long counter;
} result_t;//recording results

typedef struct {
	short valid;
	unsigned long long tag;
	unsigned long long lru;
} line_t;//make a cache line

typedef struct {
	int s_size, e_size;
	//s_size: number of set index bits
	//e_size: number of lines per set
	line_t** mycache;
} cache_t;//make a cache 

cache_t* initCache(int s, int e){//initialize a cache
	cache_t* my = (cache_t*)malloc(sizeof(cache_t));
	my->s_size = 1<<s;
	my->e_size = e;
	my->mycache = (line_t**)malloc(my->s_size * sizeof(line_t*));
	int i = 0;
	for (i = 0; i < my->s_size; i++){
		my->mycache[i] = (line_t*)malloc(my->e_size * sizeof(line_t));
	    memset(my->mycache[i], 0, my->e_size * sizeof(line_t));
	}
	return my;
}

void freeCache(cache_t* my){//free the cache
	int i = 0;
	for (i = 0; i < my->s_size; i++){
		free(my->mycache[i]);
	}
	free(my->mycache);
	free(my);
}

int get_tag(unsigned long long addr, int s, int b){
	//return the tag given address,s and b
	return(addr >> (s+b));
}

unsigned long long get_index(unsigned long long addr, int s, int b){
	//return set index given address,s and b
	return((0x7fffffffffffffff >> (63-s)) & (addr >> b));
}

void loadCache(cache_t* my, unsigned long long addr, param_t param, result_t* result){
	unsigned long long tag = get_tag(addr,param.s,param.b);
	int index = get_index(addr,param.s,param.b);
	line_t* line = my->mycache[index];
	unsigned long long hitcount = line[0].lru;
	int hit = 0;
	int hitcount_index = 0;
	int i = 0;//set things up
	for (i = 0; i < my->e_size; i++){
		//if hit, update lru
		if(line[i].valid == 1 && line[i].tag == tag){
			line[i].lru = result->counter;
			result->counter += 1;
			hit = 1;
			break;
	    }else if (line[i].valid == 0){//if not valid,set valid,add tag,update lru
            line[i].lru = result->counter;
            result->counter += 1;
            line[i].valid = 1;
            line[i].tag = tag;
            break;
        }else{
		    if(line[i].lru < hitcount){//record last lru
                hitcount = line[i].lru;
                hitcount_index = i;
			}
        }
    }
    if(hit){
		result->hits += 1;
	}else{
		result->misses += 1;
		if(i == my->e_size){//eviction,set new tag, update lru
			result->evictions += 1;
			line[hitcount_index].valid = 1;
			line[hitcount_index].tag = tag;
			line[hitcount_index].lru = result->counter;
			result->counter += 1;
		}
	}
}

void storeCache(cache_t* my, unsigned long long addr, param_t param, result_t* result){
	unsigned long long tag = get_tag(addr,param.s,param.b);
	int index = get_index(addr,param.s,param.b);
	line_t* line = my->mycache[index];
	int hit = 0;
	int i = 0;
	for (i = 0; i < my->e_size; i++){
		if(line[i].valid == 1 && line[i].tag == tag){
			line[i].lru = result->counter;
			result->counter += 1;
			hit = 1;
			break;
		}
	}
	if(hit){
		result->hits += 1;
	}else{
		loadCache(my,addr,param,result);
	}
}

void modifyCache(cache_t* my, unsigned long long addr, param_t param, result_t* result){
	loadCache(my,addr,param,result);
	storeCache(my,addr,param,result);
}
    
int main(int a, char* b[])
{
	char op;
	param_t param;
	memset(&param,0,sizeof(param));
	char* input = NULL;
	while((op = getopt(a,b,"s:E:b:t:vh"))!= -1){
		switch(op){
			case 's':
				param.s = atoi(optarg);
				break;
			case 'E':
				param.e = atoi(optarg);
				break;
			case 'b':
			    param.b = atoi(optarg);
				break;
			case 't':
				input = optarg;
				break;
			case 'v':
				param.v = 1;
				break;
			case 'h':
				usage(b[0]);
				exit(0);
			default:
				usage(b[0]);
			    exit(-1);
		}
	}
	if(param.s==0 || param.e==0 || param.b==0 || input==NULL){
		printf("%s: Hey Bro!What are you doing?\n",b[0]);
		usage(b[0]);
		exit(-1);
	}
	
	cache_t* my = initCache(param.s,param.e);
	result_t result;
	memset(&result,0,sizeof(result_t));
	result.counter = 1;
	FILE* f = fopen(input,"r");
	if(f==NULL){
		perror(input);
		exit(-1);
	}
	
	char operate[2];
	unsigned long long addr;
	int size;
	
	while(fscanf(f,"%s %llx,%d",operate,&addr,&size) != EOF){
		if(operate[0] == 'I'){
			continue;
		}
		result_t old = result;
		if(operate[0] == 'L'){
			loadCache(my,addr,param,&result);
		}else if(operate[0] == 'S'){
			storeCache(my,addr,param,&result);
		}else{
			modifyCache(my,addr,param,&result);
		}
		if(param.v){
			int misses = result.misses - old.misses;
			int hits = result.hits - old.hits;
			int evictions = result.evictions - old.evictions;
			if(misses + hits + evictions > 0){
				printf("%s %llx,%d",operate,addr,size);
				while(misses){
					printf("miss ");
					misses--;
				}
				while(evictions){
					printf("eviction ");
					evictions--;
				}
				while(hits){
					printf("hit ");
				    hits--;
				}
				printf("\n");
			}
		}
	}
	
	printSummary(result.hits, result.misses, result.evictions);
	freeCache(my);
	fclose(f);
    return 0;
}
