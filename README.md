# CSP Algorithm Notebook

用于持续积累的 CSP 算法学习笔记。默认使用 **C++17**，重点记录三件事：**什么时候能用、为什么正确、哪里容易错**。具体考试的语言标准以当次环境为准。

## 从这里开始

| 想查什么 | 入口 |
| --- | --- |
| 单组、多组、读到 EOF、整行、矩阵、图 | [输入格式模板](docs/01-input.md) |
| 容器选择、排序、查找、去重、堆 | [STL 速查](docs/02-stl.md) |
| 网格连通块、传送门、无权最短路、回溯 | [BFS / DFS](docs/03-bfs-dfs.md) |
| 有序查找、答案二分、合法边界、跳石头 | [二分](docs/04-binary-search.md) |
| 区间调度、分数背包、中位数距离和 | [贪心](docs/05-greedy.md) |
| 邻接矩阵/表、Floyd、Dijkstra、并查集、拓扑排序 | [图论](docs/06-graphs.md) |
| 静态区间和、批量区间修改、二维公式 | [前缀和 / 差分](docs/07-prefix-difference.md) |
| 质数、筛法、gcd/lcm、取模、快速幂 | [数学技巧](docs/08-math.md) |
| 根据约束筛选算法、时间和空间估算 | [复杂度与算法判断](docs/09-complexity.md) |
| 提交前检查、常见错误与复盘 | [易错点 / 错题](docs/10-pitfalls.md) |
| 选/不选、位运算、子集和、set 去重 | [状态枚举](docs/11-bitmask.md) |
| 找不小于 X 的最小质数 | [例题分析](problems/next-prime.md) · [完整程序](examples/next-prime.cpp) |
| 运行本次学习的数值与网格示例 | [完整程序与输入](#完整程序与输入) |
| 复制常用函数 | [算法模板代码](templates/algorithms.hpp) · [程序骨架](templates/main.cpp) |
| 每次学习后如何增量更新 | [更新约定](CONTRIBUTING.md) |
| 每次变动记录 | [CHANGELOG](CHANGELOG.md) |

## 先判断，再套模板

1. 看输入规模、数值范围、时间和空间限制。
2. 先想直接模拟或枚举，估算是否足够。
3. 找结构：有序或单调、重复区间查询、等权最短路、连通关系等。
4. 写出算法前提、正确性理由、复杂度。
5. 测试最小规模、边界、重复值、无解、极端数据。

“最小”“最大”本身不能说明可以二分或贪心。算法选择过程见 [复杂度与判断](docs/09-complexity.md)。

## 模板约定

- 数组和顶点编号默认从 0 开始；题目从 1 开始时，在读入处统一转换。
- 模板中的区间默认左闭右开 [l,r)，前缀和及差分保持一致。
- 总和、距离、乘法结果优先核算是否需要 long long；中间运算也要避免溢出。
- 模板头文件中的函数均在 csp 命名空间；提交时复制需要的函数及头文件。
- 文档代码片段可能依赖前文声明；完整可编译代码位于 templates/、examples/、tests/。
- 模板的适用范围和验证结果不代表学习者已掌握。个人掌握程度只根据实际反馈记录。

## 本地验证

在仓库根目录使用支持 C++17 的 g++：

~~~sh
mkdir build
g++ -std=c++17 -O2 -Wall -Wextra -pedantic tests/smoke.cpp -o build/smoke
g++ -std=c++17 -O2 -Wall -Wextra -pedantic examples/next-prime.cpp -o build/next-prime
~~~

运行 build/smoke（Windows 为 build/smoke.exe）。例题手动输入 20、2、99992，预期分别为 23、2、100003。build 已存在时无需重复创建。

## 完整程序与输入

以下程序均调用现有模板头文件，可按上面的编译命令替换源文件和输出名运行。表中顶点输入编号为 1..n，字符网格每行无空格；示例详细规则见对应主题。

| 程序 | 输入格式 | 输出 |
| --- | --- | --- |
| [grid-components.cpp](examples/grid-components.cpp) | R C，随后 R 行 01 网格 | 四方向的 1 连通块数 |
| [grid-portals.cpp](examples/grid-portals.cpp) | R C，随后含 S/T、#、.、成对字母的网格 | 最少步数；不可达为 -1 |
| [jump-stones.cpp](examples/jump-stones.cpp) | L n m，随后 n 个中间石头位置 | 最优最短跳跃距离 |
| [floyd.cpp](examples/floyd.cpp) | n m q，m 条无向非负边 u v w，q 对查询 u v | 每对的最短距离；不可达为 -1 |
| [subset-sums.cpp](examples/subset-sums.cpp) | n（0..20），随后 n 个数 | 不同子集和的数量，包含空集 |
| [fractional-knapsack.cpp](examples/fractional-knapsack.cpp) | n capacity，随后 n 对重量和价值 | 最大价值，示例保留两位小数 |
| [median-distance.cpp](examples/median-distance.cpp) | n，随后 n 个坐标 | 最小绝对距离和 |

## 后续学习

每次提供“题目或题号 + 自己的代码 + 卡住的地方 + 实际结果”，按 [CONTRIBUTING](CONTRIBUTING.md) 增量整理，并更新 CHANGELOG。

GitHub 保存的是已经提交的文件。聊天内容不会自动进入仓库；每次学习后的更新需要实际执行一次整理和提交，不承诺后台自动同步。
