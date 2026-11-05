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

## 迭代器与边界

- vector 扩容可能使已有指针、引用和迭代器失效；erase 后被删位置及后方迭代器失效。
- erase 遍历可用 it = container.erase(it)，具体以容器支持的重载为准。
- multiset.erase(value) 会删除所有等于 value 的元素；只删一个先 find，再 erase(iterator)。
- size() 通常为无符号类型；空容器不要直接计算 size()-1。
- map 查找只想判断存在时用 find，不要顺手用下标插入。
