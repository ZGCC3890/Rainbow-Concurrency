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
#include <thread>
#include <atomic>

#define LEN_DICT (122784+1)
#define LEN_RINGBUF 32000000

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

short SkipStaticPointer(int length, int dist, FSM* fsm, short state, short* stateArray, int position);
short SkipDynamicPointer(unsigned char* contents, int length, int dist, FSM* fsm, short state, short* stateArray, int position);

short SkipStaticPointer2(int length, int dist, FSM* fsm, register short state, short* stateArray, int position);
short SkipDynamicPointer2(unsigned char* contents, int length, int dist, FSM* fsm, register short state, short* stateArray, int position);

short ScanPointer(int len, int dist, int pos, short state, FSM* fsm, short* stateArray, unsigned char* content);
short ScanPointerWhile(int len, int dist, int pos, short state, FSM* fsm, short* stateArray, unsigned char* content);

int LoadText(char *path, std::vector<MemBuf> &buff, std::vector<std::string> &name);

template <typename T>
class LockFreeQueue {
public:
    explicit LockFreeQueue(size_t capacity); // 构造函数

    // 禁止拷贝和赋值
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

    // 成员函数声明
    void enqueue(const T& value);  // 入队操作
    T dequeue();                   // 出队操作
    bool isEmpty();                // 是否为空

private:
    std::vector<T> buffer;               // 队列缓冲区
    std::atomic<size_t> head, tail;      // 原子变量：队列头和尾的索引
    const size_t capacity;               // 队列容量
};

#endif