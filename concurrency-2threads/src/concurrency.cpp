//
// Created by ZGCC on 25-1-15.
//
#include "utils.h"
#include "concurrency.h"
#include <vector>
#include <iostream>

#define ACTION

std::mutex mtx;

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
                ScanByte(curState, *(text+pos), fsm_);
                stateArray_[pos + LEN_DICT] = curState;
                pos++;
#ifdef ACTION
                g_literals++;
#endif
            }
            // 读pointer给copyThread
            if (len > 0) {
                short metaState = curState;
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
//                std::cout << preState << " " << *(cur - 1) << std::endl;
                checkMeta_.enqueue({pos, metaState, meta[j], text});
                pos += len;
            }
        }
    }
    copyThreadEnd = true;
}

void checkThread(LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_){
    Messenger messenger;
    bool checkToken = false;
    bool skipped = false;
    while(true){
        if(checkMeta_.dequeue(messenger)) {
            pointer_count += messenger.meta.len;
            // 从messenger取值
            int dist = messenger.meta.dist;
            unsigned int len = messenger.meta.len;
            int pos = messenger.pos;
            short preState= messenger.preState;
            short* cur = stateArray_ + LEN_DICT + pos;
            short* refer = cur + dist - 1;
            unsigned char* token = messenger.text + pos + dist;

            if(checkToken){
                wrong_cnt++;
                unsigned int ins = messenger.meta.ins;
                unsigned char* token_t = messenger.text + pos - ins;
                cur = stateArray_ + LEN_DICT + pos - ins;
                // 将preState更新到token前一位状态重新计算token状态
                preState = *(cur - 1);
                for (int i = 0; i < ins; ++i, ++cur, ++token_t) {
                    *cur = ScanByte(preState, *token_t, fsm_);
                }
            }// 完整scan了token state，浪费

            for (int i = 0; i < len; ++i, ++cur, ++refer, ++token) {
                // 当前状态等于之前的状态，复制无误，跳过
                if(preState == *refer){
                    skipped = true;
                    break;
                }
                else{   // 否则说明需要重新计算状态
                    ++wrong_pointer_count;
                    if(dist < 0){
                        *cur = ScanByte(preState, *token, fsm_);
                    }
                    else{
                        *cur = ScanByte(preState, kBrotliDictionaryData[dist++], fsm_);
                    }
                }
            }
            if(!skipped){
                // 如果未跳过，说明pointer状态全错，需要重新计算token state
                checkToken = true;
            }else{
                checkToken = false;
                skipped = false;
            }
        }
        else if(copyThreadEnd){
            break;
        }
    }
}
//