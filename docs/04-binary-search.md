# 二分：先证明单调

完整整数模板见 [algorithms.hpp](../templates/algorithms.hpp)：first_true、last_true。数组有序查找优先使用 [STL](02-stl.md) 的 lower_bound / upper_bound。

## 什么时候能用

1. 在有序数组中查位置；或
2. 将答案转为可行性判定 ok(x)，且判定值在搜索区间内只能从 false 变成 true 一次（反向单调也可改写）。

“求最小值”不是充分条件。例如 is_prime(x) 会真假交替，不能直接二分下一个质数；见 [质数例题](../problems/next-prime.md)。

## 左闭右开模板

first_true(lo, hi, ok) 在 [lo,hi) 查找第一个 true，全部 false 返回原始 hi。模板要求 0 <= lo <= hi <= LLONG_MAX。

~~~cpp
long long answer = csp::first_true(0, limit, [&](long long x) {
    return feasible(x);
});
if (answer == limit) {
    // [0,limit) 中没有可行答案。
}
~~~

如果 limit 本身也是候选，需要在安全范围内把右端点改成 limit+1，或选择另一个已证明覆盖答案的区间，不能直接把 limit 排除掉。

循环中 mid = lo + (hi-lo)/2；ok(mid) 为真，答案不会在 mid 右侧，令 hi=mid；否则令 lo=mid+1。区间长度每次严格缩小。

## 最大可行值：闭区间 last_true

last_true(lo,hi,ok) 查找闭区间 [lo,hi] 的最后一个 true，要求 0<=lo<=hi<=LLONG_MAX、ok(lo) 已知为真，判定为 true→false。它没有“全不可行”的返回哨兵，调用前必须保证上述前提。

~~~cpp
// 核心循环；完整泛型函数见头文件。
while (lo < hi) {
    long long mid = hi - (hi - lo) / 2; // 向上取整，且不计算 hi-lo+1。
    if (ok(mid)) lo = mid;
    else hi = mid - 1;
}
~~~

维护最优答案仍在 [lo,hi] 且 lo 可行。向上取整保证 lo<hi 时 mid>lo，更新后区间严格缩小；即使 hi=LLONG_MAX 也不会因加一溢出。

| 函数 | 区间 / 真假方向 | 边界约定 |
| --- | --- | --- |
| first_true | [lo,hi)，false→true | 无解返回原始 hi；hi 不参与判定 |
| last_true | [lo,hi]，true→false | lo 必须可行，hi 可以不可行 |

## 跳石头：最大化最短跳跃距离

输入 L、n、m 和 n 个不同的中间石头位置，0<a[i]<L、L>=1、0<=m<=n；起点 0 与终点 L 不可移除。最多移除 m 块，最大化剩余相邻位置的最短距离。完整程序见 [jump-stones.cpp](../examples/jump-stones.cpp)，函数为 max_min_stone_gap。

判断距离 gap 是否可行：先排序，从起点向右扫描。当前石头离上个保留位置小于 gap 就移除，否则保留。保留最早能保留的石头，会给后续留下最多空间，贪心不会增加最少移除数。距离要求越大越难，判定为 true→false；在 [0,L] 调用 last_true，0 一定可行。

终点必须检查：若 L-last<gap，移除次数再加一，表示撤掉最后一块保留的中间石头，并非删除终点。它之前的保留间距已至少为 gap，合并末两段仍合法；gap<=L 保证无需删除起点。

边界例：L=10、石头 {4,9}、m=1，答案 4。若忽略到终点的距离，会错误地认为 gap=5 可行。还要检查 n=0、m=0、m=n、距离恰好等于 gap，以及输入未排序。

时间 O(n log n+n log(L+1))，按值接收石头副本占 O(n) 空间。示例输入 25 5 2，随后位置 2 11 14 17 21，输出 4。

## 答案二分示例：每天处理容量

设每项工作量非负，每天按原顺序处理一段，问最多 D 天完成所需的最小容量。要求 D>=1，每项不可拆分。

- 容量更大不会使原本可行的安排失效，因此 ok(capacity) 是 false→true。
- 下界为最大单项工作量，上界为总工作量。
- 判定时顺序装入，超过容量就开始下一天，统计天数是否 <=D。
- 空工作表答案为 0；总和和上界+1 都要检查溢出。
- 若单次判定 O(n)，总时间 O(n log S)，S 为搜索区间长度。

## 最大平均值：整数二分 + 最小前缀和

适用：原序列不能排序，选择长度**至少 F**的连续区间，使平均值最大；n 可达 10^5，元素在 1..2000，答案为最大平均值乘1000后向下取整。只枚举长度 F 不够，例如 [10,1,10]、F=2，最优区间长度为3。

二分整数 mid，判断是否存在合法区间满足 1000*sum/len >= mid。移项后等价于转换数组 b[i]=1000*a[i]-mid 的某个长度至少 F 的区间和非负。若 mid 可行，更小的值必然可行，因此找最后一个 true。整数二分直接得到向下取整的答案，不需要 double 或 floor。

设 pre[0]=0，pre[r] 为转换数组前 r 个元素之和。以 r 为右端点的合法区间 [l,r] 满足 l−1<=r−F，因此最大的合法区间和为 pre[r]−min(pre[0..r−F])。每当 r 增加1，只需把 pre[r−F] 加入最小前缀和候选，之前候选不删除。这样同时考虑了所有长度 >=F，而非固定窗口。

~~~cpp
// a 从1开始编号，n 和 f 已读入
bool check(int mid){
    vector<long long> pre(n+1,0);
    for(int i=1;i<=n;i++)
        pre[i]=pre[i-1]+1000LL*a[i]-mid;
    long long minPre=0;
    for(int r=f;r<=n;r++){
        minPre=min(minPre,pre[r-f]);
        if(pre[r]-minPre>=0) return true;
    }
    return false;
}
// main 中：最大可行整数
int l=0,r=2000000; // 2000*1000，不是200000
while(l<r){
    int mid=l+(r-l+1)/2;
    if(check(mid)) l=mid;
    else r=mid-1;
}
cout<<l;
~~~

时间 O(n log 2000000)，空间 O(n)；前缀和最大量级 2×10^11，使用 long long。例：a=[1,2,2]、F=3，最大平均值乘1000为1666.666…，check(1666) 真、check(1667) 假，输出1666。边界：F=1、F=n、全相等、答案恰为整数、最优区间长于F。任何区间平均值不能超过数组最大值。

## 常见边界

- 空区间、单个候选、全部可行、全部不可行。
- 下标与值域混淆；找位置还是找答案？
- 返回 end / 上界哨兵后越界解引用。
- 浮点二分不能机械使用整数的 mid+1，通常固定迭代次数并按精度要求处理。
