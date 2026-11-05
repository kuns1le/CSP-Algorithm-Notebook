# 二分：先证明单调

完整整数模板见 [algorithms.hpp](../templates/algorithms.hpp)：first_true。数组有序查找优先使用 [STL](02-stl.md) 的 lower_bound / upper_bound。

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

## 答案二分示例：每天处理容量

设每项工作量非负，每天按原顺序处理一段，问最多 D 天完成所需的最小容量。要求 D>=1，每项不可拆分。

- 容量更大不会使原本可行的安排失效，因此 ok(capacity) 是 false→true。
- 下界为最大单项工作量，上界为总工作量。
- 判定时顺序装入，超过容量就开始下一天，统计天数是否 <=D。
- 空工作表答案为 0；总和和上界+1 都要检查溢出。
- 若单次判定 O(n)，总时间 O(n log S)，S 为搜索区间长度。

## 常见边界

- 空区间、单个候选、全部可行、全部不可行。
- 下标与值域混淆；找位置还是找答案？
- 返回 end / 上界哨兵后越界解引用。
- 浮点二分不能机械使用整数的 mid+1，通常固定迭代次数并按精度要求处理。
