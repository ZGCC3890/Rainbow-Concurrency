#include "utils.h"
#include <string>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include "dictionary.h"

unsigned long g_scan = 0;
unsigned int g_match = 0;
int literal_num = 0;
clock_t spend = 0;

FSM *readFromFile(char *tableFile, char *acceptFile)
{
    // 将目标状态表acceptFile读入acceptVec
    vector<int> acceptVec;
    ifstream in_accept;
    in_accept.open(acceptFile);
    if (in_accept.is_open())
    {
        while (in_accept)
        {
            int temp_ac;
            in_accept >> temp_ac;
            acceptVec.push_back(temp_ac);
        }
        in_accept.close();
    }

    // 将状态转移表tableFile读入tableVec
    vector<int> tableVec;
    ifstream in_table;
    in_table.open(tableFile);
    if (in_table.is_open())
    {
        string line;
        while (!in_table.eof())
        {
            getline(in_table, line);
            if (in_table.fail())
                break;
            if (line.size() > 2)
            {
                stringstream stream(line);
                int count = 0;
                while (stream)
                {
                    int temp_n;
                    stream >> temp_n;
                    if (count < LENGTH_MAX)
                    {
                        tableVec.push_back(temp_n);
                    }
                    count++;
                }
            }
        }
    }

    int *list_ = new int[(int)tableVec.size()];
    bool *accept_ = new bool[(int)tableVec.size() / LENGTH_MAX];
    // 初始accept_为false
    for (int i = 0; i < (int)tableVec.size() / LENGTH_MAX; i++)
    {
        accept_[i] = false;
    }
    for (int i = 0; i < tableVec.size(); i++)
    {
        list_[i] = tableVec[i];
    }
    for (auto it = acceptVec.begin(); it != acceptVec.end(); it++)
    {
        accept_[*it] = true;
    }
    FSM *fsm = new FSM;
    fsm->list = list_;
    fsm->accept = accept_;  
    return fsm;
}

// input即输入文本 rule即状态转移表 accept即pattern结束状态表
int main(int argc, char **argv)
{
    char *input = argv[1];
    char *rule = argv[2];
    char *accept = argv[3];
    FSM *fsm = readFromFile(rule, accept);
    vector<m_buffer> buff;
    int size = LoadText(input, buff); // load data
    int *sizArray = new int[size];
    unsigned char **Txt = new unsigned char *[size];
    Init(buff, Txt, sizArray);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long start = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    short state = 0;
    for (int i = 0; i < size; i++)
    {
        unsigned char *token = Txt[i];
        int len = sizArray[i];
        state = 0;
        for (int j = 0; j < len; j++)
        {
            state = fsm->list[state * LENGTH_MAX + token[j]];
            if(fsm->accept[state]) g_match++;
            g_scan++;
        }
    }
    gettimeofday(&tv, NULL);
    long end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    printf("state = %d\n", state);
    printf("g_scan = %d\n", g_scan);
    printf("time = %d ms\n", end - start);
    printf("match = %d\n", g_match);
    delete[] Txt;
}