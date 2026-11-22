# STL 速查

复杂度以下表中的单次操作为准；unordered 容器是平均复杂度，最坏情况可能退化。

| 工具 | 常用操作 | 复杂度与提醒 |
| --- | --- | --- |
| vector | push_back、pop_back、a[i]、size | 尾插均摊 O(1)；中间插删 O(n)；下标访问不检查越界 |
| string | size、substr、find | substr 复制长度 k 的子串为 O(k)；find 的失败值是 string::npos |
| deque | push_front、push_back、pop_front | 两端插删 O(1)，可随机访问 |
| stack | push、top、pop | 默认底层容器下 O(1)；pop 不返回值 |
| queue | push、front、pop | 默认底层容器下 O(1)，用于 BFS |
| priority_queue | push、top、pop | 入堆/出堆 O(log n)，取顶 O(1)；默认大根堆 |
| set / multiset | insert、find、lower_bound | O(log n)；set 去重，multiset 保留重复值 |
| map | find、count、m[key] | O(log n)；m[key] 会在键不存在时插入默认值 |
| unordered_map / unordered_set | insert、find、erase | 平均 O(1)，最坏 O(n)；无排序、需可哈希的键 |
| pair | first、second | 默认按 first，再按 second 作字典序比较 |

包含对应标准头文件。常用算法在 algorithm；accumulate、gcd、lcm 在 numeric。

## 排序和比较器

~~~cpp
sort(a.begin(), a.end()); // O(n log n)，默认升序。
sort(a.begin(), a.end(), greater<int>()); // 降序，元素类型应匹配。
sort(items.begin(), items.end(), [](const auto& x, const auto& y) {
    if (x.first != y.first) return x.first < y.first;
    return x.second < y.second;
});
~~~

比较器必须满足严格弱序，不能写成 <=。需要保留相等元素原顺序时使用 stable_sort。

## 全排列：next_permutation

适用：需要按**字典序**枚举所有排列，且 `n!` 的规模允许。

~~~cpp
vector<int> a={1,2,3};
sort(a.begin(),a.end()); // 从最小排列开始
do{
    // 处理当前排列 a
}while(next_permutation(a.begin(),a.end()));
~~~

`next_permutation(first,last)` 会**修改原数组**，产生紧接着的下一个字典序排列；返回 `false` 时表示已到最大排列，并把数组重排成最小排列。使用 `do...while` 才不会漏掉初始排列。每次 O(n)，枚举所有排列共 O(n·n!)。

固定从城市 1 出发又回到 1 的旅行商枚举：只排列其余 `n-1` 个城市，共 `(n-1)!` 种；路径和要包含**起点到第一站、相邻站、最后一站回起点**三部分。

## 二分查找：先保证有序

~~~cpp
auto l = lower_bound(a.begin(), a.end(), x); // 第一个 >= x。
auto r = upper_bound(a.begin(), a.end(), x); // 第一个 > x。
auto count_x = r - l;                       // x 的出现次数。
bool exists = l != a.end() && *l == x;       // end 不能解引用。
~~~

vector 上为 O(log n)。set/map 使用容器自己的 lower_bound，为 O(log n)；通用 lower_bound 在非随机访问迭代器上可能进行线性次迭代器移动。查找比较顺序须与排序一致。

## 排序去重与删除

~~~cpp
sort(a.begin(), a.end());
a.erase(unique(a.begin(), a.end()), a.end());
a.erase(remove(a.begin(), a.end(), x), a.end());
~~~

unique 只把相邻重复元素移到有效区间外，不会改变容器 size；remove 也需要配合 erase。

## 小根堆

~~~cpp
priority_queue<long long, vector<long long>, greater<long long>> pq;
pq.push(5);
pq.push(2);
auto minimum = pq.top(); // 2。调用前先保证非空。
pq.pop();
~~~

pair 堆可用于 Dijkstra，默认先比较距离再比较顶点。

## 求和、最值、计数

~~~cpp
long long sum = accumulate(a.begin(), a.end(), 0LL);
auto it = min_element(a.begin(), a.end());
if (it != a.end()) cout << *it << '\n';
auto cnt = count(a.begin(), a.end(), x);
~~~

accumulate 的初值决定累加类型，求 long long 和时不要写 0。min_element 对空区间返回 end。

## map 统计频次：键是否存在与出现次数

~~~cpp
map<long long,int> freq;
for (long long x : a) ++freq[x]; // 统计时有意创建缺少的键。
auto it = freq.find(x);         // 查询不改变容器。
int occurrences = it == freq.end() ? 0 : it->second;
~~~

map 的 count(x) 只返回键是否存在（0 或 1），原数组中的频次是 it->second。freq[x] 在键不存在时会插入默认值 0；仅查询用 find。设有 K 个不同的键，建表 O(n log(K+1))、每次查询 O(log(K+1))、空间 O(K)。

std::find 在 vector 上线性扫描；map.find 使用容器索引。值域小且非负时可用频次数组。一次查找还是先排序/建表，结合查询次数判断，见 [复杂度与查找取舍](09-complexity.md#枚举还是建立查找结构)。

## 本阶段新用法：fill、array、pair 解包

### fill：重置已有元素

~~~cpp
vector<int> dist(n+1);
fill(dist.begin(),dist.end(),-1);    // [begin,end) 全部设为 -1
fill(dist.begin()+1,dist.end(),0);   // 只设置下标 1..n
~~~

`fill(first,last,x)` 定义在 `<algorithm>`，O(区间长度)，**不改变容器大小**。与 `resize(n,x)`（改变长度，只为新增元素填 x）、`assign(n,x)`（替换成 n 个 x）区分。

### array<int,3>：固定长度的三元组

~~~cpp
vector<array<int,3>> edges;          // 保存 {权重,起点,终点}
edges.push_back({10,1,2});
edges.push_back({5,2,3});
sort(edges.begin(),edges.end());     // 默认按第0项，再第1项、第2项升序
for(auto [w,u,v]:edges){ /* 使用三个字段 */ }
~~~

`array<T,3>` 是 STL 固定长度容器；`pair<T,U>` 存两个字段。C++17 的 `auto [v,w]` 称为**结构化绑定**，适合遍历 `vector<pair<...>>` 与 `vector<array<...>>`。

### priority_queue<pair<...>>：Dijkstra 常用小根堆

~~~cpp
priority_queue<pair<long long,int>,
               vector<pair<long long,int>>,
               greater<pair<long long,int>>> pq;
pq.push({7,3});  // {距离,顶点}
pq.push({2,5});
auto [d,u]=pq.top(); // d=2,u=5；堆顶最小
pq.pop();
~~~

堆**不是排好序的数组**；仅保证堆顶是最小值。旧的距离记录不会自动删除，Dijkstra 通过 `if(d!=dist[u]) continue;` 跳过过期记录。

## 迭代器与边界

- vector 扩容可能使已有指针、引用和迭代器失效；erase 后被删位置及后方迭代器失效。
- erase 遍历可用 it = container.erase(it)，具体以容器支持的重载为准。
- multiset.erase(value) 会删除所有等于 value 的元素；只删一个先 find，再 erase(iterator)。
- size() 通常为无符号类型；空容器不要直接计算 size()-1。
