# 图论

> 各类最短路算法的适配题型、原理和核心 C++ 语句集中整理在 [最短路专题](13-shortest-path.md)。

完整代码见 [algorithms.hpp](../templates/algorithms.hpp)：DSU、dijkstra、floyd、topo_sort；bfs 和 dfs_order 见 [搜索笔记](03-bfs-dfs.md)。

## 先问清图的性质

有向还是无向？是否带权？边权能否为负？有无自环和重边？是否连通？顶点编号范围？稀疏还是稠密？

| 需求 | 首先考虑 | 限制与复杂度 |
| --- | --- | --- |
| 可达性、无向连通块 | BFS / DFS | 邻接表 O(V+E) |
| 最少边数 / 等权最短路 | BFS | 各边代价相同 |
| 非负权单源最短路 | 堆 Dijkstra | 本模板 O((V+E)log(V+E))；简单图常写 O((V+E)log V) |
| 有负边的单源最短路 | Bellman-Ford 等 | Bellman-Ford O(VE)，需判断可达负环；本仓库暂未提供此模板 |
| 小规模所有点对最短路 | Floyd | O(V³) 时间、O(V²) 空间；本模板使用非负边权 |
| 合并集合、无向连通关系 | 并查集 | 路径压缩+按大小合并，均摊 O(α(V)) |
| 带权无向图的最小连接成本 | Kruskal 最小生成树 | 边排序 + 并查集；选中不足 V−1 条边则不连通 |
| 依赖顺序、有向无环图判定 | 拓扑排序 | O(V+E)；输出不足 V 个顶点说明有环 |
| 相邻点必须不同色 | 二分图染色 | BFS / DFS，O(V+E) |
| 两类点最大一对一配对 | DFS 增广路 | 详见二分图专题 |

## 存储

| 存储 | 空间 | 查一条边 | 遍历一个点的邻居 |
| --- | --- | --- | --- |
| 邻接表 | O(V+E) | 通常 O(deg(u)) | O(deg(u)) |
| 邻接矩阵 | O(V²) | O(1) | O(V) |

稀疏图常用邻接表，无向边存两次。规模小、经常查指定两点间边时可用矩阵。带权矩阵不能把无边都设为 0，否则与零权边混淆。

~~~cpp
csp::WeightedGraph g(n);
g[u].push_back({v, w});
g[v].push_back({u, w}); // 无向图才添加反向边。
auto dist = csp::dijkstra(g, source);
if (dist[target] == csp::INF) {
    // 不可达。
}
~~~

Dijkstra 模板在堆里放 (距离,顶点)，取出后跳过过期距离。所有边权必须非负；有限最短路须严格小于 INF，模板会保护距离加法。图的顶点数至少为 1，起点和所有边的端点必须有效。

## Floyd：任意两点最短路

~~~cpp
vector<vector<csp::ll>> d(n, vector<csp::ll>(n, csp::INF));
for (int i=0; i<n; ++i) d[i][i]=0;
d[u][v]=min(d[u][v], w); // 对每条边执行；重边保留最小权。
auto distances=csp::floyd(d);
~~~

本模板限定初始有限边权在 [0,csp::INF) 内、所有有限最短路严格小于 csp::INF；无向图还要更新反向边，自环也通过 min 保持正确初值。不可达保留 INF。

令 d[i][j] 表示仅允许已处理顶点作为中间点时的最短路。处理 k 时，最短路或者不经过 k，或者分成 i→k 和 k→j，因此更新为 min(d[i][j], d[i][k]+d[k][j])。**k 必须在最外层**。先排除不可达，再相加；本模板的有限值均小于 INF=LLONG_MAX/4，两项相加不会溢出。

时间 O(V³)、矩阵空间 O(V²)，按值传入会复制矩阵。算法理论上也可处理无负环的负边图，但不属于本函数约定，需另行核算负环与数值边界。

完整程序 [floyd.cpp](../examples/floyd.cpp)：输入 n m q、m 条无向非负边 u v w、q 对查询顶点；输入编号为 1..n，读入时转成 0-based。输出最短距离，不可达为 -1。边界覆盖单点、重边、零权边、自环、断开图和大于 int 的距离。

## 并查集

~~~cpp
csp::DSU dsu(n);
dsu.unite(u,v);
bool connected = dsu.same(u,v);
~~~

适合只增加连接的集合合并。普通并查集不支持任意删边，也不能直接判断有向可达性。find 的代表元不保证是最小编号。

最小生成树是并查集的典型应用，具体原理及普通 vector 实现见下文「Kruskal：最小生成树（MST）」。

## 拓扑排序

~~~cpp
auto order = csp::topo_sort(g); // g 是有向的无权邻接表。
bool acyclic = order.size() == g.size();
~~~

每次取入度为 0 的点，删除其出边。多个入度为 0 的点意味着顺序可能不唯一。若要求字典序最小，把队列改为小根堆并重新核算复杂度。

## 新学算法：最短路、连通块与拓扑排序

### Dijkstra：非负边权的单源最短路

固定起点 `s`；`dist[v]` 为到 `v` 的当前最短候选距离。小根堆存 `{距离,顶点}`，反复松弛邻边；`d!=dist[u]` 表示堆记录已过期。允许重边和零权边，**不能直接用于负权边**。

~~~cpp
const long long INF=1LL<<60;
vector<long long> dist(n+1,INF);
priority_queue<pair<long long,int>,vector<pair<long long,int>>,
               greater<pair<long long,int>>> pq;
dist[s]=0; pq.push({0,s});
while(!pq.empty()){
    auto [d,u]=pq.top(); pq.pop();
    if(d!=dist[u]) continue;
    for(auto [v,w]:g[u]){  // g[u] 中是 {终点,非负边权}
        if(dist[v]>d+w){
            dist[v]=d+w;
            pq.push({dist[v],v});
        }
    }
}
~~~

邻接表 + 堆：时间 O((N+M)log(N+M))，空间 O(N+M)。若题目要求不可达输出 `2^31-1`，用 `INT_MAX`（`<climits>`），而非 `pow(2,31)-1`。

**路径收益建模**：若下坡收益为海拔差，上坡损失为海拔差的2倍，则可把有向代价改为 `w(u,v)=max(0LL,H[v]-H[u])`；最大幸福感为 `max_v(H[1]-H[v]-dist[v])`，再用 Dijkstra 求最小累计上坡代价。

### 删除每条边后的无权最短路：只重算一条原最短路径上的边

初次 BFS 求 1→N 的距离 D，并用 `preEdge[v]` 记录到达 `v` 的边编号；邻接表 `g[u]` 应保存边编号，边列表 `edge[id]={起点,终点}`。

- 不在所选最短路径上的边，删除后原路径仍可用，所以答案仍为 D。
- 只对这条路径上的边，禁用该边重新 BFS。
- 若原图不可达，则全部输出 -1。

~~~cpp
int D=bfs(-1,true);  // ban=-1：不删边，true：记录 preEdge
vector<int> ans(m,D);
if(D!=-1){
    int cur=n;
    while(cur!=1){
        int id=preEdge[cur];
        ans[id]=bfs(id,false);    // 仅禁用编号 id 的边
        cur=edge[id].first;       // 回溯到前驱顶点
    }
}
~~~

一次 BFS 为 O(N+M)，最短路径至多 N-1 条边，总时间 O(N(N+M))、空间 O(N+M)；若第一次找不到路径，不能回溯 `preEdge[n]`。

### 并查集：不使用 struct 的基本模板

~~~cpp
vector<int> parent,sz;
int findRoot(int x){
    return parent[x]==x ? x : parent[x]=findRoot(parent[x]);
}
void unite(int a,int b){
    a=findRoot(a); b=findRoot(b);
    if(a==b) return;
    if(sz[a]<sz[b]) swap(a,b);
    parent[b]=a;
    sz[a]+=sz[b];
}
// 初始化：parent.resize(n+1); sz.assign(n+1,1);
// for(int i=1;i<=n;i++) parent[i]=i;
~~~

只有**根节点**的 `sz[root]` 表示集合大小；两个不同连通块合并时，新增跨集合顶点对数为 `sz[ra]*sz[rb]`。

| 题型 | 顺序与贡献 |
| --- | --- |
| 依次拆桥后的不连通点对数 | **倒序恢复**；初始 `ans=1LL*n*(n-1)/2`；先记录当前答案，再对不同集合执行 `ans-=1LL*sz[ra]*sz[rb]` 和合并 |
| 树上所有点对的路径**最大边权之和** | 边存为 `array<int,3>{w,u,v}`，按权重**升序**；加入边时 `ans+=1LL*w*sz[ra]*sz[rb]`，再合并 |
| 员工共享语言可间接交流 | `first[lang]` 存最先掌握该语言的员工；后续员工同语言即合并；若至少有人懂语言，答案是连通块数-1；若所有人都不懂，答案是 n |

按边权升序的理由：两个连通块**第一次连通**时，当前边权就是它们之间所有新连通点对的路径最大边权。两种合并计数都必须**先乘旧集合大小，再合并**。并查集只维护合并，不支持一般的在线删边。

### Kruskal：最小生成树（MST）

**适配题型。** 给定带权无向图，选出一些边连接所有顶点，使保留的边权之和最小且不形成环。例如最小建路费用、铺设网络线路；图不连通时不存在生成树。最小生成树的目标是整个图的连接成本，而不是某个起点到其他点的最短距离，也不同于哈夫曼树的最小带权路径长度。

**原理。** 生成树有 n 个顶点、恰好 n−1 条边。将所有边按权值从小到大排序，每次选取两个端点尚未连通的最便宜的边，并用并查集合并这两个连通块。如果两端已经连通，加入该边会形成环，因此跳过。

为什么可以贪心？当前已选边构成若干连通块。连接不同连通块的最便宜的边满足最小生成树的割性质：若某个包含此前已选边的最优生成树没有这条边，可以加入它并删去环上另一条跨越同一个割、权值不小于它的边。替换后仍是生成树，总代价不增加。因此逐次选择这样的边可以得到最优解。

**关键实现（直接使用普通 vector 和上面的 findRoot 函数）。**

~~~cpp
// n 个顶点，m 条无向边；每条边只需存一次
vector<array<int,3>> edges;
for(int i=0;i<m;i++){
    int u,v,w;
    cin>>u>>v>>w;
    edges.push_back({w,u,v}); // 权值在前，sort 自动按边权升序
}
sort(edges.begin(),edges.end());

parent.resize(n+1);
sz.assign(n+1,1);
for(int i=1;i<=n;i++) parent[i]=i;

long long ans=0;
int cnt=0;                   // 已选边数
for(auto [w,u,v]:edges){     // 与 {w,u,v} 的存储顺序一致
    int ru=findRoot(u);
    int rv=findRoot(v);
    if(ru==rv) continue;     // 已经连通，再加会成环

    if(sz[ru]<sz[rv]) swap(ru,rv);
    parent[rv]=ru;           // 小集合挂到大集合
    sz[ru]+=sz[rv];          // 无论是否交换，都要执行合并

    ans+=w;
    cnt++;
    if(cnt==n-1) break;
}
if(cnt==n-1) cout<<ans;
else cout<<"orz";            // 具体无解输出由题目规定
~~~

**如何判断图不连通？** 初始化时有 n 个连通块，每次成功选边就减少一个连通块。选中 n−1 条边后只剩一个连通块；如果扫描完仍不足 n−1 条，就不存在生成树。单个顶点的生成树权值为 0。

**常见变形。** 若题目要求在原图连通、所有边权非负的前提下，删除总权值尽可能大的边并保持连通，可先统计原图边权总和 sum，再求最小生成树权值 ans，答案为 sum−ans。若允许负权边且只要求连通而不要求保留部分为树，不能直接套这一结论：保留额外负权边可能更优。

**边界与注意。** 零权边和负权边都可以进入最小生成树，不要因权值小于等于0就跳过；重边可直接参与排序，较便宜的边优先；自环的两个端点本来就在同一个集合，会被跳过。并查集交换根节点的 if 只决定合并方向，真正修改 parent 和更新 sz 必须放在 if 外面。

**复杂度。** 排序 O(m log m)，路径压缩与按大小合并的并查集操作总计 O(m α(n))，含初始化为 O(n+m log m+m α(n))，空间 O(n+m)。一般主要开销是边排序。Prim 也是最小生成树算法，但采用从一个已选顶点集合向外扩展的方式；本节以已经学习的 Kruskal 为主。

### 拓扑排序 Kahn：字母表顺序约束

对相邻名字找**第一个不同字符**，建立 `u→v` 表示 u 必须排在 v 前；重复边只添加一次。若没有不同字符且前一个名字更长，直接 `Impossible`。

~~~cpp
vector<vector<int>> g(26);
vector<int> indeg(26,0);
bool has[26][26]={};
// 比较相邻字符串：找到第一个不同字符 u,v 后
if(!has[u][v]){
    has[u][v]=true;
    g[u].push_back(v);
    indeg[v]++;
}
~~~

~~~cpp
queue<int> q;
string order;
for(int i=0;i<26;i++) if(indeg[i]==0) q.push(i);
while(!q.empty()){
    int u=q.front(); q.pop();
    order+=char('a'+u); // 出队才计入答案
    for(int v:g[u])
        if(--indeg[v]==0) q.push(v);
}
// order.size()==26 且无前缀冲突才有解
~~~

入度为0表示所有前置约束已满足；不足26个字母说明有向环。O(总字符串长度+26²) 时间，额外图空间 O(26²)。

### 树上固定中转点 K 的路径距离

树上两点间简单路径唯一。若查询允许经过 K 后折返，先以 K 为起点 BFS/DFS **累加实际边权**求 `dist[v]`，每次答案即 `dist[x]+dist[y]`。预处理 O(N)，Q 次查询 O(Q)，无需 Dijkstra。

### 二分图染色与最大匹配

这两个问题的目标不同：染色是让每条边的两个端点不同色，最大匹配是选择尽可能多的互不共享端点的边。河蟹封锁道路、DFS 增广路与网格马赛克建模已单独整理在 [二分图专题](14-bipartite.md)，包括普通 vector 写法和正确性说明。

## 边界检查

孤立点、断开的图、自环、重边、零权边、从起点不可达的点；并查集重复合并；拓扑排序的有向环。
