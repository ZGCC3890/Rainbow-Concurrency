//
// Created by ZGCC on 25-1-15.
//
#include "utils.h"
#include "concurrency.h"
#include <vector>
#include <iostream>

//#define ACTION

void scanThread(LockFreeQueue<Messenger> &checkMeta_, int count, std::vector<MemBuf> &contents_, std::vector<MemBuf> &metaInput_, int *metaSize_, short curState, short *stateArray_, FSM* fsm_){
    for (int i = 0; i < count; i++) {
        unsigned char* text = contents_[i].pBuff;
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
                if (fsm_->accept[curState]) g_match++;
#endif
            }
            // 读pointer给copyThread
            if (len > 0) {
                checkMeta_.enqueue({pos, curState, meta[j], text});
                int dist = meta[j].dist;
                if (dist < 0) {
//                    curState = SkipDynamicPointer(text, len, dist, fsm_, curState, stateArray_, pos);
                    // 复制状态
                    short *cur = stateArray_ + LEN_DICT + pos;
                    short *refer = cur + dist - 1;
                    if (cur - refer >= len) {
                        memcpy(cur, refer + 1, sizeof(short) * len);
                    } else {
                        for (int k = 0; k < len; k++) {
                            cur[k] = refer[k + 1];
                        }
                    }
                    curState = cur[len - 1];
                }
                else
                {
//                    curState = SkipStaticPointer(len, dist, fsm_, curState, stateArray_, pos);
                    short* refer = stateArray_ + dist - 1;
                    short* cur = stateArray_ + LEN_DICT + pos;
                    memcpy(cur, refer + 1, sizeof(short) * len);
                    curState = cur[len - 1];
                }
                pos += len;
            }
        }
    }
    copyThreadEnd = true;
}

void copyThread(LockFreeQueue<Messenger> &copyMeta_, LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_) {
    Messenger messenger;
    while (true) {
        if(copyMeta_.dequeue(messenger)) {
            int dist = messenger.meta.dist;
            unsigned int len = messenger.meta.len;
            int pos = messenger.pos;
            if (dist < 0)
            {
//                messenger.curState = SkipDynamicPointer(messenger.text, len, dist, fsm_, messenger.curState, stateArray_, pos);
                // 复制状态
                short *cur = stateArray_ + LEN_DICT + pos;
                short *refer = cur + dist - 1;
                if (cur - refer >= len)
                {
                    memcpy(cur, refer + 1, sizeof(short) * len);
                }
                else
                {
                    for (int i = 0; i <= len; i++)
                        *(cur+i) = *(refer+i);
                    *cur = *refer;
                }
            }
            else{
//                messenger.curState = SkipStaticPointer(len, dist, fsm_, messenger.curState, stateArray_, pos);
                short* refer = stateArray_ + dist - 1;
                short* cur = stateArray_ + LEN_DICT + pos;
                memcpy(cur, refer + 1, sizeof(short) * len);
                for (int p = 0; p < len; p++, cur++)
                {
                    *cur = messenger.curState;
                    messenger.curState = ScanByte(messenger.curState, kBrotliDictionaryData[dist++], fsm_);
                }
            }
            checkMeta_.enqueue(messenger);
        }
        else if(scanThreadEnd){
            copyThreadEnd = true;
            break;
        }
    }
}


void checkThread(LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_){
    Messenger messenger;
    while(true){
        if(checkMeta_.dequeue(messenger)) {
            int dist = messenger.meta.dist;
            unsigned int len = messenger.meta.len;
            int pos = messenger.pos;
            short* cur = stateArray_ + LEN_DICT + pos;
            short* refer = cur + dist - 1;
            unsigned char* token = messenger.text + pos + dist;
            for (int i = 0; i < len; ++i, ++cur, ++refer, ++token) {
                // 当前状态等于之前的状态，复制无误，跳过
                if(messenger.curState == *refer){
#ifdef ACTION
                    for (int j = 0; j < len - i; j++) {
                        if (fsm_->accept[cur[j + i]]) g_match++;
                    }
#endif
                    break;
                }
                else{   // 否则说明需要重新计算状态
                    if(dist < 0){
                        *cur = ScanByte(messenger.curState, *token, fsm_);
                    }
                    else{
                        *cur = ScanByte(messenger.curState, kBrotliDictionaryData[dist++], fsm_);
                    }
#ifdef ACTION
                    if (fsm_->accept[*cur]) g_match++;
#endif
                }
            }
        }
        else if(copyThreadEnd){
            break;
        }
    }
}