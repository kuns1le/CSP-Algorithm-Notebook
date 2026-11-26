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

## 连续区间划分：各段 OR，再把结果 XOR

按位或 |：对应位至少一个1则为1；按位异或 ^：对应位不同则为1。累积 OR 和 XOR 都可从0开始，a^a=0。C++ 的 |、^、& 是位运算，不是逻辑运算 ||、&&。

适配：N<=20，将整个序列分成一个或多个非空连续区间，每段求按位 OR，再求这些结果的 XOR，最小化最终值。

关键不是枚举选哪些元素，而是枚举**相邻元素之间的 N−1 个间隙**。第 i 位为1表示在 a[i] 后切割，为0表示与 a[i+1] 同段。每个 mask 唯一表示一种划分，共 2^(N−1) 种。mask=0 是整段，并不是空方案。

~~~cpp
int ans=INT_MAX;
for(int mask=0;mask<(1<<(n-1));mask++){
    int curOR=0,curXOR=0;
    for(int i=0;i<n;i++){
        curOR|=a[i];
        if(i<n-1 && (mask&(1<<i))){
            curXOR^=curOR;
            curOR=0; // 只清空当前段，不能清空历史 XOR
        }
    }
    curXOR^=curOR; // 最后一段没有切割位，必须单独合并
    ans=min(ans,curXOR);
}
~~~

例：[1,5,7]，在第二个元素后切割，得到 (1|5)^7=2。时间 O(N·2^(N−1))，额外空间 O(1)。N=1 时仅有 mask=0，结果就是唯一元素。全部为0、全切、不切、重复元素都需正确处理。

## 复杂度与边界

设不同的和有 K 个：每个状态扫描 n 位，再进行 set 插入，时间 O(2^n·(n+log(K+1)))，结果空间 O(K)，K<=2^n。若连同输入数组计算，空间 O(n+K)。

检查空数组（仅有和 0）、全零、重复元素、负数和大数求和。与 [STL 去重](02-stl.md)、[复杂度估算](09-complexity.md) 配合使用。这里只提供模板与本地验证，不记录为个人已经掌握或在线通过。
