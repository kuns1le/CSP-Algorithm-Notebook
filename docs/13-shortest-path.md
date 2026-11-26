# 最短路专题：算法选型、原理与核心写法

> C++17 学习笔记。以**适配题型、原理、关键语句**为主；代码段只表示关键思路，不要求独立编译。默认顶点为 1..n。不使用自定义命名空间、类或完整算法封装。

## 先判断题型

| 要求 / 图的性质 | 首选算法 | 核心理由 |
| --- | --- | --- |
| 无权、每一步成本相同；求最少步数 | **BFS** | 队列分层，第一次发现就是最少步数 |
| 边权非负，求固定起点到其他点的最短路 | **Dijkstra** | 优先处理当前距离最小的顶点 |
| 存在负边，单源最短路或检测**可达负环** | **Bellman-Ford** | 反复扫描所有边松弛 |
| 有负边，希望只处理距离发生变化的顶点 | **SPFA** | 队列优化 Bellman-Ford，但可能被卡 |
| 顶点较少，求**任意两点**的最短路 | **Floyd-Warshall** | 三重循环的全点对动态规划 |
| 点多、边少、允许负权，求**所有点对**最短路 | **Johnson** | 全图判负环，重新赋权后跑 n 次 Dijkstra |
| 加权**树**，固定起点到其他点的距离 | **DFS / BFS 累加** | 树上简单路径唯一，没必要用 Dijkstra |

负边不等于负环；**负环**是边权和小于 0 的有向环。在负环能影响的起终点间，没有有限最短路。单源通常只关心从起点可达的负环；题目若要求“图中存在任何负环就输出 -1”，必须进行**全图**检测。

### 两种存图方式

~~~cpp
// 邻接表：BFS、Dijkstra、SPFA 常用
vector<vector<pair<int,int>>> g(n+1);
g[u].push_back({v,w}); // {终点,边权}；无向图需再存反向边

// 边集：Bellman-Ford 每轮遍历全部边更方便
vector<array<int,3>> edges;
edges.push_back({u,v,w}); // {起点,终点,边权}
~~~

图中可能存在重边、自环。注意区分：**邻接表的 pair 为 {终点,边权}；Dijkstra 堆的 pair 为 {距离,顶点}**。

## 1. BFS：无权图 / 等权最短路

**适用题型：** 普通无权图两点间最少经过几条边、迷宫最少步数、八数码最少移动次数。所有转移等成本时，最少步数即最小代价；边权不相等时不能直接用普通 BFS。

**为什么正确？** 队列遵循先进先出。起点距离为 0，起点的邻居距离为 1，再向外扩展距离为 2…… 因而状态**第一次被发现**时已经由最少步数到达它。入队时记录距离，防止同一状态被重复加入。

~~~cpp
vector<int> dist(n+1,-1); // -1 表示未访问
queue<int> q;
dist[s]=0; q.push(s);
while(!q.empty()){
    int u=q.front(); q.pop();
    for(int v:g[u]){ // 此处使用无权邻接表 vector<vector<int>>
        if(dist[v]!=-1) continue;
        dist[v]=dist[u]+1;
        q.push(v);
    }
}
~~~

**状态 BFS 的特别注意：** 八数码的状态必须包含**整个棋盘排列**，而不只是空格位置；用 `unordered_set<string>` 保存完整字符串状态可以避免往返搜索。复杂度：普通邻接表图 O(n+m)，空间 O(n+m)（含图）。

## 2. Dijkstra：非负权单源最短路

**适用题型：** 非负路费、非负耗时、最小累计代价；后面 Johnson 重赋权后的图。**不能直接用于含负边的普通最短路问题**。

**为什么正确？** `dist[u]` 是从起点到 u 的当前最短候选值。每次从**小根堆**取出候选距离最小的顶点 u，遍历其出边尝试松弛。由于所有边权非负，其他更远的路径不可能绕回来改进这个已经以最小值取出的有效状态。

松弛公式：`dist[v] = min(dist[v], dist[u]+w)`。

~~~cpp
const long long INF=1LL<<60;
vector<long long> dist(n+1,INF);
priority_queue<pair<long long,int>,vector<pair<long long,int>>,
               greater<pair<long long,int>>> pq;
dist[s]=0;
pq.push({0,s});             // {距离,顶点}
while(!pq.empty()){
    auto [d,u]=pq.top(); pq.pop();
    if(d!=dist[u]) continue; // 旧堆记录已经过期
    for(auto [v,w]:g[u]){     // {终点,非负边权}
        if(dist[v]>d+w){
            dist[v]=d+w;
            pq.push({dist[v],v});
        }
    }
}
~~~

**为什么有过期记录？** 可能先向堆插入 `{10,3}`，之后又找到更短路径插入 `{5,3}`；堆不会自动删除旧元素。`d != dist[u]` 就是跳过旧的 10。注意不能把堆写成 `{顶点,距离}`，否则它会优先选择编号小的顶点，而不是距离短的顶点。

复杂度：邻接表与二叉堆通常写作 O((n+m)log(n+m))，空间 O(n+m)。

## 3. Bellman-Ford：允许负边，检测负环

**适用题型：** 负权单源最短路、从指定起点能否到达负环；经过收益建模后，还可以求最大收益并判定可达**正环**是否使收益无限。

**为什么不是只遍历一轮？** 一条边 `u→v` 被扫描时，可能还不知道 u 的最短距离。例如链 `1→2→3→4` 的边按 `3→4、2→3、1→2` 的逆序存放：第一轮只能更新 2，第二轮更新 3，第三轮才更新 4。若边顺序正好相反，原地更新可能一轮传到终点。为了不依赖输入顺序，需要**至多 n−1 轮**：没有可达负环时，最短路径可以选不重复顶点的简单路径，至多 n−1 条边。

~~~cpp
const long long INF=1LL<<60;
vector<long long> dist(n+1,INF);
dist[s]=0;
for(int i=1;i<n;i++){
    bool changed=false;
    for(auto [u,v,w]:edges){
        if(dist[u]==INF) continue; // 起点不可达就不能扩展
        if(dist[v]>dist[u]+w){
            dist[v]=dist[u]+w;
            changed=true;
        }
    }
    if(!changed) break; // 整轮无变化，已收敛
}
~~~

**如何判断可达负环？** 正常 n−1 轮后，再遍历一次所有边：如果还满足 `dist[u]!=INF && dist[v]>dist[u]+w`，说明能继续降低某条可达路径的距离，因此存在**从 s 可达**的负环。不是简单检查单条边 `w<0`。

~~~cpp
bool negativeCycle=false;
for(auto [u,v,w]:edges){
    if(dist[u]!=INF && dist[v]>dist[u]+w){
        negativeCycle=true;
        break;
    }
}
~~~

**如果要检测全图负环？** 增加虚拟源点0，向每个原顶点连权值0的边；可以直接把所有 `h[i]` 初始化为0，等价于先松弛完虚拟边。只遍历原图边做**最多 n 轮**；第 n 轮仍有更新即存在全图负环，这正是 Johnson 的第一步。

**本阶段遇过的最大收益题：** 普通路净收益 `D`，机票费用 T 则净收益 `D-T`，起点收益设 `gain[S]=D`，其他设为负无穷。把松弛改为 `gain[v]=max(gain[v],gain[u]+w)`，第 n 轮仍能增大收益则有可达正环，可以无限赚钱。**负净收益边仍可能通往更赚钱的城市，不可直接跳过。**

复杂度：O(nm) 时间、O(n+m) 空间（含边集）。

## 4. SPFA：队列优化 Bellman-Ford

**适用题型：** 有负边的单源最短路和可达负环检测，尤其每次变化只影响少数顶点的情况。它**不是必然比 Bellman-Ford 快**；最坏可退化到 O(nm) 量级，题目明确卡 SPFA 时不应强行使用。

**设计动机：** Bellman-Ford 反复扫描所有边，但当 `dist[u]` 未变化时，以同样距离反复检查 u 的出边通常没有意义。SPFA 把**距离刚被有效更新、需要再次展开出边的顶点**放入普通队列，只处理它们。

- `inq[u]`：顶点**此刻是否在队列中**。出队必须改回 `false`，以后如果距离再次变小才能再次入队。
- `v` 已在队列中时，若 `dist[v]` 变小不必重复入队；出队时使用的自然是最新距离。
- `cnt[v]`：本次形成 `dist[v]` 的路径**边数**，不是“入队次数”。更新 `u→v` 后令 `cnt[v]=cnt[u]+1`。若 `cnt[v]>=n`，则检测到起点可达的负环。
- **只有成功松弛才尝试入队**；负环检测成立后需要退出外层 while。

~~~cpp
vector<long long> dist(n+1,INF);
vector<int> cnt(n+1,0);
vector<bool> inq(n+1,false);
queue<int> q;
dist[s]=0; q.push(s); inq[s]=true;
bool negativeCycle=false;

while(!q.empty()){
    int u=q.front(); q.pop();
    inq[u]=false;                    // 出队后将来仍可入队
    for(auto [v,w]:g[u]){
        if(dist[v]<=dist[u]+w) continue; // 只有严格变小才松弛
        dist[v]=dist[u]+w;
        cnt[v]=cnt[u]+1;
        if(cnt[v]>=n){negativeCycle=true;break;}
        if(!inq[v]){q.push(v);inq[v]=true;}
    }
    if(negativeCycle) break;
}
~~~

**与 BFS、Dijkstra 的区别：** BFS 中首次发现就有最少步数；SPFA 中一个顶点可以反复入队（前面的路径未必最优）；Dijkstra 使用按距离排序的小根堆，而 SPFA 的普通队列只是等待处理的顶点列表。

## 5. Floyd-Warshall：小图全点对最短路

**适用题型：** n 较小、询问大量点对或直接需要 n×n 的距离表。可处理负边，但若有负环，涉及负环的最短距离没有有限值。

**原理（动态规划）：** 固定编号 k，考虑是否让 k 作为路径的中间点。`i→j` 的最优路线，要么不使用 k，要么从 i 到 k 再从 k 到 j，所以：
 
> `d[i][j] = min(d[i][j], d[i][k]+d[k][j])`

**k 必须在最外层**，表示依次允许更多顶点作为中间点。

~~~cpp
vector<vector<long long>> d(n+1,vector<long long>(n+1,INF));
for(int i=1;i<=n;i++) d[i][i]=0;
for(auto [u,v,w]:edges)
    d[u][v]=min(d[u][v],(long long)w); // 重边取较小边权

for(int k=1;k<=n;k++)
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++){
            if(d[i][k]==INF || d[k][j]==INF) continue;
            d[i][j]=min(d[i][j],d[i][k]+d[k][j]);
        }
// 结束后若存在 d[i][i]<0，则图中有负环
~~~

复杂度：O(n³) 时间，O(n²) 空间。n=3000 时约 270 亿次三重循环迭代，通常不适合；n 为几十到几百更常见。

## 6. Johnson：有负边的稀疏图全点对最短路

**适用题型：** 既要处理**负权边**，又要求**所有起点**到全部终点的距离，n 较大但 m 较小。例如你练过的 n≤3000、m≤6000 的题目。

**核心思路：Bellman-Ford 一次 + 重新赋权 + n 次 Dijkstra。**

**步骤① 用虚拟起点计算势函数 h[]，并检测全图负环。** 虚拟源点0到每个原顶点都存在一条零权边；不真正建这些边，而将所有 `h[i]` 初始化为0，原图边松弛最多 n 轮。如果第 n 轮仍更新，说明原图某处存在负环；整轮未更新则提前停止。

~~~cpp
vector<long long> h(n+1,0);
for(int i=1;i<=n;i++){
    bool changed=false;
    for(auto [u,v,w]:edges){
        if(h[v]>h[u]+w){
            h[v]=h[u]+w;
            changed=true;
        }
    }
    if(!changed) break;
    if(i==n){ /* 全图存在负环：按题目输出 -1 并结束 */ }
}
~~~

**步骤② 重新赋权，让所有边非负。** 对原边 u→v，旧权 w，定义：

> `w' = w + h[u] − h[v]`

为什么一定非负？因为 Bellman-Ford 已经收敛，不存在可继续松弛的边，故 `h[v] <= h[u]+w`，移项得到 `w+h[u]-h[v]>=0`。

~~~cpp
vector<vector<pair<int,long long>>> g(n+1);
for(auto [u,v,w]:edges){
    long long nw=1LL*w+h[u]-h[v];
    g[u].push_back({v,nw});
}
~~~

**步骤③ 为什么不会改变最短路径？** 从 s 到 t 的路径所有新边权相加时，中间顶点的 `+h` 与 `−h` 相互抵消，因此：

> **新路径长度 = 原路径长度 + h[s] − h[t]**

相同起终点的所有路径都增加**同一个常数**，原来最短的路线仍然最短。

**步骤④ 每个 s 运行 Dijkstra，并恢复原距离：**

> `原距离(s,t) = 新距离(s,t) − h[s] + h[t]`

新图边权全非负，所以可用 Dijkstra。如果 t 不可达，必须按题目的要求单独处理，不可对 INF 套恢复公式。对于“不可达距离记为10^9、输出终点编号加权和”的练习题：

~~~cpp
long long ans=0;
for(int t=1;t<=n;t++){
    if(dis[t]==INF){
        ans+=1LL*t*1000000000;
    }else{
        long long realDist=dis[t]-h[s]+h[t];
        ans+=1LL*t*realDist;
    }
}
~~~

其中 `dis[t]` 为**本轮从 s 出发的 Dijkstra 在新图中的距离**，不是原距离。注意**可达分支也要乘终点编号 t**。

复杂度：Bellman-Ford O(nm)，n 次堆 Dijkstra 约 O(n(n+m)log(n+m))，总体 O(nm+n(n+m)log(n+m))；稀疏图中常明显优于 Floyd。

## 7. 你做过的变形题与易错点

**已遇到的变形。** 八数码需要“完整状态 + BFS + 哈希”；删去每条边求无权最短路时，只需要对某条已知最短路径上的边重新 BFS（其他边被删除后该路径仍存在）；树上定点 K 中转距离可用从 K 累加的树上距离 `dist[x]+dist[y]`（允许折返）；最大收益题使用 Bellman-Ford 的 max 松弛和**可达正环**检测。

**提交前按这个顺序检查：**

1. **先看方向**：有向边只加单向；无向边必须存两个方向。有的题规定 w≥0 双向、w<0 单向，要严格按题意建图。
2. **看 pair 的元素顺序**：邻接表 `{终点,边权}`；Dijkstra 堆 `{距离,顶点}`。
3. **看能否有负边/负环**：有负边不能直接 Dijkstra；负边不等于负环。单源判可达负环，全源判全图负环。
4. **看不可达**：BF 必须过滤 `dist[u]==INF`；Floyd 防止把 INF 相加；Johnson 的 INF 不能直接恢复。题目规定不可达为10^9时按它输出，不是按内部 INF。
5. **看更新条件**：最短路用 `>`；最大收益用 `<`。SPFA 只有成功松弛才入队；出队执行 `inq[u]=false`；Bellman-Ford 可以通过 `changed` 提前退出。
6. **看整数范围、重边、自环**：距离和加权总和用 `long long`，乘法用 `1LL`；Floyd 用 `min` 处理重边；负权自环就是负环。

**一句话记忆：等权 BFS；非负 Dijkstra；负边 Bellman-Ford / SPFA；小图全源 Floyd；大稀疏负权全源 Johnson。**
