#include "utils.h"
#include <unistd.h>
#include <string>
#include <algorithm>
#include <sys/time.h>

using namespace std;

// _input即解压后明文 _meta即metaData文件 rule即状态转移表 accept即结束状态 set即数据集 rounds即循环次数
int main(int argc, char **argv)
{
    char *_input = argv[1];
    char *_meta = argv[2];
    char *rule = argv[3];
    char *accept = argv[4];
    int set = atoi(argv[5]);
    int rounds = atoi(argv[6]);

    if (set == 1)	// Alexa
    {
        total = 73385030;
        compress = 12397003;
    }
    else if (set == 2) // Google
    {
        total = 226060405;
        compress = 53840635;
    }
    else if (set == 3) // Enwik
    {
        total = 100000000;
        compress = 25742001;
    }
    else if (set == 4) // COVID-19
    {
        total = 113096550;
        compress = 7364753;
    }
    else if (set == 5) // Malicious
    {
        total = 152429387;
        compress = 24068963;
    }

    FSM *fsm = readFsmTable(rule, accept);

    vector<MemBuf> contents;
    vector<MemBuf> metaInput;

    vector<string> InputName;
    vector<string> MetaName;

    readFileName(_input, InputName);
    readFileName(_meta, MetaName);

    //排序
    sort(InputName.begin(), InputName.end());
    sort(MetaName.begin(), MetaName.end());

    // sleep(3600);

    for (int i = 0; i < InputName.size(); i++)
    {
        int read = 0;
        struct stat statbuf;
        string input = InputName[i];
        stat(input.c_str(), &statbuf);
        unsigned char *pBuf = (unsigned char *)malloc(statbuf.st_size);
        FILE *hFile = fopen(input.c_str(), "rb");
        read = fread(pBuf, statbuf.st_size, 1, hFile);
        contents.push_back({pBuf, static_cast<size_t>(statbuf.st_size)});

        string metaFile = MetaName[i];
        stat(metaFile.c_str(), &statbuf);
        unsigned char *pBuf1 = (unsigned char *)malloc(statbuf.st_size);
        hFile = fopen(metaFile.c_str(), "rb");
        read = fread(pBuf1, statbuf.st_size, 1, hFile);
        fclose(hFile);
        metaInput.push_back({pBuf1, static_cast<size_t>(statbuf.st_size)});
    }

    short *stateArray = new short[LEN_DICT + LEN_RINGBUF];
    GetDictionaryState(stateArray, fsm);

    int count = contents.size();
    int *metaSize = new int[count];
    for (int i = 0; i < count; i++)
        metaSize[i] = metaInput[i].size / sizeof(MetaData);

    short state = 0;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    long start = tv.tv_sec*1000 + tv.tv_usec/1000;
    for(int r=0; r<rounds; r++)
    {
        for (int i = 0; i < count; i++)
        {
            unsigned char *text = contents[i].pBuff;
            MetaData *meta = (MetaData*)metaInput[i].pBuff;
            int pos = 0;
            state = 0;

            for (int j = 0; j < metaSize[i]; j++)
            {
                unsigned int ins = meta[j].ins;
                unsigned int len = meta[j].len;
                for (int k=0; k<ins; k++)
                {
                    ScanByte(state, text[pos], fsm);
                    stateArray[pos + LEN_DICT] = state;
                    pos++;

#ifdef ACTION                    
                    g_literals++;
#endif
                }
                // scan pointer
                if (len > 0)
                {
        //           state = ScanPointerWhile(len, meta[j].dist, pos, state, fsm, stateArray, text);
        //           state = ScanPointer(len, meta[j].dist, pos, state, fsm, stateArray, text);
        
                    int dist = meta[j].dist;
                    if (dist < 0)
                    {
                        state = SkipDynamicPointer(text, len, dist, fsm, state, stateArray, pos);
                    }
                    else
                    {
                        state = SkipStaticPointer(len, dist, fsm, state, stateArray, pos);
                    }
       
                    pos += len;
#ifdef ACTION
                    g_pointer_len += len;
                    g_pointer_count++;
#endif
                }
            }
        }
    }
    gettimeofday(&tv,NULL);
    long end = tv.tv_sec*1000 + tv.tv_usec/1000;
    g_spend = end - start;
    printf("state = %d\n", state);
    printPerformance(rounds);

    delete[] fsm->accept;
    delete[] fsm->list;
    delete[] stateArray;
    delete fsm;
    delete [] metaSize;

    for (int i = 0; i < contents.size(); i++)
    {
        delete contents[i].pBuff;
        delete metaInput[i].pBuff;
    }
}
