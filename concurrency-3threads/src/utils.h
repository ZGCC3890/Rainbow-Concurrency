#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <fstream>
#include <sstream>

#define LEN_DICT (122784+1)
#define LEN_RINGBUF 32000000

extern bool threadEnd;
extern unsigned long g_literals;
extern unsigned long g_spend;
extern unsigned long g_scan;
extern unsigned long g_pointer_len;
extern unsigned long g_pointer_count;
extern unsigned long g_match;
extern unsigned long long compress;
extern unsigned long long total;
extern unsigned char kBrotliDictionaryData[];

typedef struct
{
	unsigned int ins;
	unsigned int len;
	int dist;
}MetaData;

typedef struct
{
	unsigned char* pBuff;
	size_t size;
}MemBuf;

typedef struct _FSM {
	int* list;
	bool* accept;
}FSM;

inline int ScanByte(short &state, unsigned char token, FSM* fsm) {
	state = fsm->list[state * 256 + token];
#ifdef ACTION
	if (fsm->accept[state]) g_match++;
	g_scan++;
#endif
	return state;
};

void printPerformance(int rounds);
int readFileName(char *path, std::vector<std::string> &name);
FSM* readFsmTable(char* tableFile, char* acceptFile);
void GetDictionaryState(short* DictionaryState, FSM* fsm);

short SkipStaticPointer(unsigned int length, int dist, FSM* fsm, short state, short* stateArray, int position);
short SkipDynamicPointer(unsigned char* contents, int length, int dist, FSM* fsm, short state, short* stateArray, int position);

short SkipStaticPointer2(int length, int dist, FSM* fsm, register short state, short* stateArray, int position);
short SkipDynamicPointer2(unsigned char* contents, int length, int dist, FSM* fsm, register short state, short* stateArray, int position);

short ScanPointer(int len, int dist, int pos, short state, FSM* fsm, short* stateArray, unsigned char* content);
short ScanPointerWhile(int len, int dist, int pos, short state, FSM* fsm, short* stateArray, unsigned char* content);

int LoadText(char *path, std::vector<MemBuf> &buff, std::vector<std::string> &name);

#endif