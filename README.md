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

## <h2 id = "naive">naive</h2>