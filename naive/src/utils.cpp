#include "utils.h"
#include <sys/time.h>
#include <dirent.h>

unsigned long g_spend = 0;
unsigned long g_literals = 0;
unsigned long g_pointer_len = 0;
unsigned long g_pointer_count = 0;
unsigned long g_scan = 0;
unsigned long g_match = 0;
unsigned long long total = 0;
unsigned long long compress = 0;

using namespace std;

void GetDictionaryState(short *stateDict, FSM *fsm)
{
	const unsigned char *symbol = kBrotliDictionaryData;
	short state = 0;
	for (int i = 0; i < LEN_DICT; i++)
	{
		state = fsm->list[state*256 + *symbol++];
		*stateDict++ = state;
	}
}

void printPerformance(int rounds)
{
	printf("time:%ld ms\n", g_spend/=rounds);
	printf("matched:%ld\n", g_match/=rounds);
	printf("scan:%ld\n", g_scan/=rounds);
	printf("total:%lld\n", total);
	printf("skipped ratio: %.2f%%\n", 100 * (1.0 - (g_scan * 1.0 / total)));
	printf("throughput %.2f mbps\n", (compress * 8) / (double)(g_spend) / 1000);
	printf("literals: %ld\n", g_literals/=rounds);
	printf("pointer ratio: %.2f%%\n", 100 * (1.0 - (g_literals * 1.0 / total)));
	printf("average pointer length = %.2f\n", (g_pointer_len * 1.0) / g_pointer_count);
}

int readFileName(char *path,vector<string> &name)
{
	struct stat statbuf;
	string szDir = "";
	szDir += path;
	DIR *pDir = opendir(szDir.c_str());
	struct dirent *entry;
	while ((entry = readdir(pDir)) != NULL)
	{
		string s = szDir;
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		s += entry->d_name;
		name.push_back(s);
	}
	closedir(pDir);
	return 0;
}

int LoadText(char *path, vector<MemBuf> &buff, vector<string> &name)
{
	struct stat statbuf;
	char szDir[512] = {0};
	strcpy(szDir, path);
	DIR *pDir = opendir(szDir);
	struct dirent *entry;
	char szFile[512];
	while ((entry = readdir(pDir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		sprintf(szFile, "%s/%s", szDir, entry->d_name);
		unsigned long dwRead = 0;
		stat(szFile, &statbuf);
		unsigned char *pBuf = (unsigned char *)malloc(statbuf.st_size);
		FILE *hFile = fopen(szFile, "rb");
		if (hFile == NULL)
		{
			printf("open file %s error\n", entry->d_name);
			continue;
		}
		int read = fread(pBuf, statbuf.st_size, 1, hFile);
		fclose(hFile);
		buff.push_back({pBuf, static_cast<size_t>(statbuf.st_size)});
	}
	closedir(pDir);
	return buff.size();
}

short SkipStaticPointer(int length, int dist, FSM* fsm, short state, short* stateArray,int position)
{
	short* refer = stateArray + dist - 1;
	short* cur = stateArray + LEN_DICT + position;
	for (int pos = 0; pos < length; pos++, refer++, cur++)
	{
//		printf("posi:%d, pos:%d, state:%d\n", position, pos, state);
		if (state == *refer)
		{
			memcpy(cur, refer + 1, sizeof(short) * (length - pos));
#ifdef ACTION
			for (int i = 0; i < length - pos; i++) {
				if (fsm->accept[cur[i]]) g_match++;
			}
#endif
			return cur[length - pos - 1];
		}
		else
		{
			*cur = ScanByte(state, kBrotliDictionaryData[dist++], fsm);
		}
	}
	return state;
}

short SkipDynamicPointer(unsigned char* contents, int length, int dist, FSM* fsm, short state, short* stateArray, int position)
{
	short* cur = stateArray + LEN_DICT + position;
	short* refer = cur + dist - 1;
	unsigned char* token = contents + position + dist; //refer str
	for (int pos = 0; pos < length; pos++, refer++, cur++,token++)
	{
		if (state == *refer)
		{
			if (cur - refer >= length)
			{
				memcpy(cur, refer + 1, sizeof(short) * (length - pos));
			}
			else {
				for (int i = 0; i < length - pos; i++) {
					cur[i] = refer[i + 1];
				}
			}
#ifdef ACTION
			for (int i = 0; i < length - pos; i++) {
				if (fsm->accept[cur[i]]) g_match++;
			}
#endif
			return cur[length - pos - 1];
		}
		else
		{
			*cur = ScanByte(state, *token, fsm);
		}
	}
	return state;
}

short ScanPointer(int len, int dist, int idx, short state, FSM* fsm, short* stateArray, unsigned char* content)
{
	unsigned char* symbol = content + idx;
	short* refer = NULL;
	short* cur = stateArray + LEN_DICT + idx - 1;

	if(dist < 0) // referring to sliding window
	{
		refer = cur + dist;
	//	symbol = content + idx;
		if (dist+len <= 0)
		{
			memcpy(cur, refer, sizeof(short)*(len+1));
		}
		else
		{
			for (int i=1; i<=len; i++)
				*(cur+i) = *(refer+i);
			*cur = *refer;
		}
	}
	else // referring to static dictionary
	{
		refer = stateArray + dist - 1;
	//	symbol = &kBrotliDictionaryData[dist];
		memcpy(cur, refer, sizeof(short)*(len+1));
	}

	for (int pos=0; pos<len; pos++, cur++)
	{
		//printf("posi:%d, pos:%d, state:%d\n", idx, pos, state);
		if (state == *cur)
		{
#ifdef ACTION
			for (int i=1; i <= len-pos; i++)
				if (fsm->accept[cur[i]]) g_match++;
#endif
			return cur[len-pos];
		}
		else
		{
			*cur = state;
			state = ScanByte(state, *symbol++, fsm);
		}
	}
	*cur = state;
	return state;
}



short SkipStaticPointer2(int length, int dist, FSM* fsm, register short state, short* stateArray,int position)
{
	short* refer = stateArray + dist - 1;
	short* cur = stateArray + LEN_DICT + position-1;
	memcpy(cur, refer, sizeof(short) * (length+1));
	for (int pos = 0; pos < length; pos++, refer++, cur++)
	{
//		printf("posi:%d, pos:%d, state:%d\n", position, pos, state);
		if (state == *cur)
		{
#ifdef ACTION
			for (int i = 1; i <= length - pos; i++) {
				if (fsm->accept[cur[i]]) g_match++;
			}
#endif
			return cur[length - pos];
		}
		else
		{
			*cur = state;
			state = ScanByte(state, kBrotliDictionaryData[dist++], fsm);
		}
	}
	*cur = state;
	return state;
}

short SkipDynamicPointer2(unsigned char* contents, int length, int dist, FSM* fsm, short state, short* stateArray, int position)
{
	short* cur = stateArray + LEN_DICT + position-1;
	short* refer = cur + dist;
	unsigned char* token = contents + position; //refer str
		if (dist+length <= 0)
		{
			memcpy(cur, refer, sizeof(short)*(length+1));
		}
		else
		{
			for (int i=1; i<=length; i++)
				*(cur+i) = *(refer+i);
			*cur = *refer;
		}
	for (int pos = 0; pos < length; pos++, cur++,token++)
	{
		if (state == *cur)
		{
#ifdef ACTION
			for (int i = 1; i <= length - pos; i++) {
				if (fsm->accept[cur[i]]) g_match++;
			}
#endif
			return cur[length - pos];
		}
		else
		{
			*cur = state;
			state = ScanByte(state, *token, fsm);
		}
	}
	*cur = state;
	return state;
}

FSM *readFsmTable(char *tableFile, char *acceptFile)
{
	vector<int> acceptVec;
	ifstream in_ac;
	in_ac.open(acceptFile);
	if (in_ac.is_open())
	{
		while (in_ac)
		{
			int temp_ac;
			in_ac >> temp_ac;
			acceptVec.push_back(temp_ac);
		}
		in_ac.close();
	}
	vector<int> vecTable;
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
					if (count < 256)
					{
						vecTable.push_back(temp_n);
					}

					count++;
				}
			}
		}
	}
	int *list_ = new int[(int)vecTable.size()];
	bool *accept_ = new bool[(int)vecTable.size() / 256];
	for (int i = 0; i < (int)vecTable.size() / 256; i++)
	{
		accept_[i] = false;
	}
	for (int i = 0; i < vecTable.size(); i++)
	{
		list_[i] = vecTable[i];
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