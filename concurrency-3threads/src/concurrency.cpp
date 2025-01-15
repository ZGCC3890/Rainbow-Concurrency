//
// Created by ZGCC on 25-1-15.
//
#include "utils.h"
#include "concurrency.h"
#include <vector>
#include <iostream>
#define ACTION

void scanThread(LockFreeQueue<Messenger> &copyMeta_, int count, std::vector<MemBuf> &contents_, std::vector<MemBuf> &metaInput_, int *metaSize_, short curState, short *stateArray_, FSM* fsm_){
    for (int i = 0; i < count; i++) {
        unsigned char *text = contents_[i].pBuff;
        MetaData *meta = (MetaData *) metaInput_[i].pBuff;
        int pos = 0;
        curState = 0;

        // 依次对一个metaData进行处理
        for (int j = 0; j < metaSize_[i]; j++) {
            unsigned int ins = meta[j].ins;
            unsigned int len = meta[j].len;

            // 处理token
            for (int k = 0; k < ins; k++) {
                ScanByte(curState, text[pos], fsm_);
                stateArray_[pos + LEN_DICT] = curState;
                pos++;
#ifdef ACTION
                g_literals++;
#endif
            }
            // 读pointer给copyThread
            if (len > 0) {
                copyMeta_.enqueue({pos, curState, text, meta[j]});
                pos += len;
            }
        }
    }
    threadEnd = true;
}

void copyThread(LockFreeQueue<Messenger> &copyMeta_, short *stateArray_, FSM* fsm_) {
//    curState = ScanPointerWhile(len, meta[j].dist, pos, state, fsm, stateArray, text);
//    curState = ScanPointer(len, meta[j].dist, pos, state, fsm, stateArray, text);
    Messenger messenger;
    while (true) {
        if(copyMeta_.dequeue(messenger)) {
            int dist = messenger.meta.dist;
            unsigned int len = messenger.meta.len;
            if (dist < 0) {
                messenger.curState = SkipDynamicPointer(messenger.text, len, dist, fsm_, messenger.curState,
                                                        stateArray_,
                                                        messenger.pos);
            } else {
                messenger.curState = SkipStaticPointer(len, dist, fsm_, messenger.curState, stateArray_, messenger.pos);
            }
#ifdef ACTION
            g_pointer_len += len;
            g_pointer_count++;
#endif
        }
        else if(threadEnd) break;
    }
}


void checkThread(){

}