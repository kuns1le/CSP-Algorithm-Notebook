# 位运算与状态枚举

## 适用条件与状态含义

n 个位置各有“选/不选”两种选择，且 2^n 个状态能在时限和内存内处理时，可以用整数的第 i 位表示是否选择下标 i。它枚举下标集合；两个值相等的元素仍是两个不同位置。

~~~cpp
bool chosen = ((mask >> i) & 1ULL) != 0;
auto added = mask | (1ULL << i);
auto removed = mask & ~(1ULL << i);
auto toggled = mask ^ (1ULL << i);
~~~

移位数必须非负且小于类型位宽。1<<n 使用 int，1ULL<<n 才使用 unsigned long long；即使能表示很多位，也不代表能枚举这么多状态。

## 示例：不同子集和，用 set 去重

完整函数 distinct_subset_sums 见 [algorithms.hpp](../templates/algorithms.hpp)，可运行程序见 [subset-sums.cpp](../examples/subset-sums.cpp)。本模板要求 0<=n<=20，所有子集和均可用 long long 表示。

枚举 mask 从 0 到 2^n-1，每个二进制模式恰好对应一个下标子集，因此不漏子集；mask=0 表示空集，其和为 0。对选中位置求和，再插入 set，让产生相同结果的不同子集合并。

~~~cpp
auto sums = csp::distinct_subset_sums({1,1,2});
// {0,1,2,3,4}，不同的和有 5 个。
~~~

程序输入 n 和 n 个数，输出不同子集和的数量。例如：

~~~text
3
1 1 2
~~~

输出 5。不要先对原数组去重：去重后的 {1,2} 会丢失选择两个 1 的状态，可能改变可得到的和。

## 复杂度与边界

设不同的和有 K 个：每个状态扫描 n 位，再进行 set 插入，时间 O(2^n·(n+log(K+1)))，结果空间 O(K)，K<=2^n。若连同输入数组计算，空间 O(n+K)。

检查空数组（仅有和 0）、全零、重复元素、负数和大数求和。与 [STL 去重](02-stl.md)、[复杂度估算](09-complexity.md) 配合使用。这里只提供模板与本地验证，不记录为个人已经掌握或在线通过。
