# Rainbow-Concurrency
A concurrency version of Rainbow
## 总览
### 输入参数  
1. input即输入文本  
2. rule即状态转移表  
3. accept即pattern结束状态表  
### 子项目
1. concurrency-3threads  
三线程并发：scan -> copy -> check
2. concurrency-2threads  
两线程并发：scan & copy -> check
3. naive  
原生Rainbow
4. naive-3threads  
原生三线程：naive * 3  
### 目录
<li><a href="#concurrency-3threads">concurrency-3threads</a> </li>
<li><a href="#concurrency-2threads">concurrency-2threads</a> </li>
<li><a href="#naive-3threads">naive-3threads</a> </li>
<li><a href="#naive">naive</a> </li>

## <h2 id = "concurrency-3threads">concurrency-3threads</h2>
### scanThread
即主线程，负责扫描当前字段是token还是pointer，是pointer就丢给copy线程复制状态  
```cpp
void scanThread(LockFreeQueue<Messenger> &copyMeta_, int count, std::vector<MemBuf> &contents_, std::vector<MemBuf> &metaInput_, int *metaSize_, short curState, short *stateArray_, FSM* fsm_);
```
int count - pointer总数  
LockFreeQueue<Messenger> &copyMeta_ - 通信用无锁队列，scanThread存入 copyThread取出  
std::vector<MemBuf> &contents_ - 文本内容  
std::vector<MemBuf> &metaInput_ - metaData输入缓冲区      
int &#42;metaSize_ - metaData长度数组  
short curState - 当前字符状态  
short&#42; stateArray_ - 状态数组  
FSM&#42; fsm_ - 状态转移表和结束状态表
### copyThread
复制线程，dist < 0 则从滑动窗口复制状态，dist > 0 从静态字典复制
```cpp
void copyThread(LockFreeQueue<Messenger> &copyMeta_, LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_);
``` 
LockFreeQueue<Messenger> &copyMeta_ - 通信用无锁队列，scanThread存入 copyThread取出  
LockFreeQueue<Messenger> &checkMeta_ - 通信用无锁队列，copyThread存入 checkThread取出  
short&#42; stateArray_ - 状态数组  
FSM&#42; fsm_ - 状态转移表和结束状态表
### checkThread
检查线程，如果当前状态和refer一致说明复制正确跳过检查
```cpp
void checkThread(LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_);
``` 
LockFreeQueue<Messenger> &checkMeta_ - 通信用无锁队列，copyThread存入 checkThread取出  
short&#42; stateArray_ - 状态数组  
FSM&#42; fsm_ - 状态转移表和结束状态表
### ISSUE
- scanThread以上一pointer指向的token最后一位状态为初始状态计算后续state，但可能错误取到copyThread尚未计算的状态0之后计算出错误状态1  
- checkThread无法获知当前pointer被哪些token取用过导致发现状态复制错误后只能更正pointer不能更正token部分state
## <h2 id = "concurrency-2threads">concurrency-2threads</h2>
### scanThread
```cpp
void scanThread(LockFreeQueue<Messenger> &checkMeta_, int count, std::vector<MemBuf> &contents_, std::vector<MemBuf> &metaInput_, int *metaSize_, short curState, short *stateArray_, FSM* fsm_);
```
int count - pointer总数  
LockFreeQueue<Messenger> &checkMeta_ - 通信用无锁队列，copyThread存入 checkThread取出    
std::vector<MemBuf> &contents_ - 文本内容  
std::vector<MemBuf> &metaInput_ - metaData输入缓冲区  
int &#42;metaSize_ - metaData长度数组  
short curState - 当前字符状态  
short&#42; stateArray_ - 状态数组  
FSM&#42; fsm_ - 状态转移表和结束状态表
### copyThread
```cpp
void checkThread(LockFreeQueue<Messenger> &checkMeta_, short *stateArray_, FSM* fsm_);
```
LockFreeQueue<Messenger> &checkMeta_ - 通信用无锁队列，copyThread存入 checkThread取出  
short&#42; stateArray_ - 状态数组  
FSM&#42; fsm_ - 状态转移表和结束状态表
## <h2 id = "naive">naive</h2>