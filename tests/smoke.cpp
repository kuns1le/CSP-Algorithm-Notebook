#include "../templates/algorithms.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
using namespace std;
using namespace csp;

int main() {
    Graph g{{1,2},{0,3},{0},{1},{}};
    assert((bfs(g,0)==vector<int>{0,1,1,2,-1}));
    auto order=dfs_order(g,0);
    sort(order.begin(),order.end());
    assert((order==vector<int>{0,1,2,3}));
    assert((bfs(Graph(1),0)==vector<int>{0}));
    assert((dfs_order(Graph(1),0)==vector<int>{0}));

    assert(grid_components({})==0);
    assert(grid_components({"1"})==1);
    assert(grid_components({"0"})==0);
    assert(grid_components({"10","01"})==2);
    assert(grid_components({"11111"})==1);
    assert(grid_components({"1100","0101","0011"})==2);
    assert(grid_portal_distance({"ST"})==1);
    assert(grid_portal_distance({"S..","##T"})==3);
    assert(grid_portal_distance({"S#T"})==-1);
    assert(grid_portal_distance({"S.A##","#####","A..T#"})==5);
    assert(grid_portal_distance({"S.A","##.","A.T"})==4);

    // All possible switch positions, including all-false and all-true.
    for (int n=0;n<60;++n)
        for (int cut=0;cut<=n;++cut)
            assert(first_true(0,n,[&](ll x){return x>=cut;})==cut);

    for (int n=0;n<60;++n)
        for (int cut=0;cut<=n;++cut)
            assert(last_true(0,n,[&](ll x){return x<=cut;})==cut);
    const ll largest=numeric_limits<ll>::max();
    assert(last_true(0,largest,[](ll){return true;})==largest);
    assert(last_true(0,largest,[&](ll x){return x<largest;})==largest-1);
    assert(max_min_stone_gap(largest,{},0)==largest);
    assert(max_min_stone_gap(25,{21,2,14,11,17},2)==4);

    // Exhaust all interior-stone sets and all permitted removal counts, L<=10.
    // The reference enumerates every retained subset, independently of the greedy check.
    int stone_cases=0;
    for (int length=1;length<=10;++length) {
        for (int mask=0;mask<(1<<(length-1));++mask) {
            vector<ll> stones;
            for (int x=1;x<length;++x)
                if (mask&(1<<(x-1))) stones.push_back(x);
            int n=static_cast<int>(stones.size());
            vector<ll> best(n+1,0);
            for (int keep=0;keep<(1<<n);++keep) {
                int removed=n;
                ll last=0, gap=length;
                for (int j=0;j<n;++j) if (keep&(1<<j)) {
                    --removed;
                    gap=min(gap,stones[j]-last);
                    last=stones[j];
                }
                gap=min(gap,static_cast<ll>(length)-last);
                best[removed]=max(best[removed],gap);
            }
            for (int m=1;m<=n;++m) best[m]=max(best[m],best[m-1]);
            for (int m=0;m<=n;++m) {
                assert(max_min_stone_gap(length,stones,m)==best[m]);
                ++stone_cases;
            }
        }
    }

    assert(max_nonoverlap({})==0);
    assert(max_nonoverlap({{1,3},{2,4},{3,5},{-2,1}})==3);
    assert(fractional_knapsack(0,{{2,5}})==0);
    assert(fractional_knapsack(100,{})==0);
    assert(fractional_knapsack(50,{{10,60},{20,100},{30,120}})==240);
    assert(fractional_knapsack(100,{{10,60},{20,100},{30,120}})==280);
    assert(abs(fractional_knapsack(1,{{3,1}})-1.0L/3)<1e-12L);
    assert(fractional_knapsack(1,{{1000000000,1000000000},{1,2}})==2);
    assert(min_absolute_distance_sum({})==0);
    assert(min_absolute_distance_sum({7})==0);
    assert(min_absolute_distance_sum({3,3,3})==0);
    assert(min_absolute_distance_sum({1,2,8,10})==15);
    assert(min_absolute_distance_sum({-1000000000,-1000000000,
                                      1000000000,1000000000})==4000000000LL);
    assert((distinct_subset_sums({})==set<ll>{0}));
    assert((distinct_subset_sums({0,0})==set<ll>{0}));
    assert((distinct_subset_sums({1,1,2})==set<ll>{0,1,2,3,4}));
    assert((distinct_subset_sums({-1,1})==set<ll>{-1,0,1}));
    DSU dsu(5);
    assert(dsu.unite(0,1)); assert(dsu.unite(1,2));
    assert(!dsu.unite(0,2)); assert(dsu.same(0,2)); assert(!dsu.same(0,4));
    WeightedGraph wg{{{1,5},{2,1}},{{3,0}},{{1,1},{3,8}}, {}, {}};
    assert((dijkstra(wg,0)==vector<ll>{0,2,1,2,INF}));
    bool rejected=false;
    try { dijkstra(WeightedGraph{{{0,-1}}},0); }
    catch (const invalid_argument&) { rejected=true; }
    assert(rejected);
    Graph dag{{1,2},{3},{3},{}};
    auto topo=topo_sort(dag);
    assert(topo.size()==4);
    vector<int> pos(4);
    for (int i=0;i<4;++i) pos[topo[i]]=i;
    for (int u=0;u<4;++u) for (int v:dag[u]) assert(pos[u]<pos[v]);
    assert(topo_sort(Graph{{1},{0}}).size()<2);
    assert(topo_sort(Graph{}).empty());

    // Random differential check against direct range addition and summation.
    mt19937 rng(20261105);

    // Compare all-pairs Floyd with independent single-source Dijkstra runs.
    int floyd_cases=0;
    assert(floyd({}).empty());
    assert((floyd({{0,INF-1},{INF,0}})==vector<vector<ll>>{{0,INF-1},{INF,0}}));
    assert(floyd({{0,3000000000LL,INF},{INF,0,3000000000LL},
                  {INF,INF,0}})[0][2]==6000000000LL);
    for (int n=1;n<=7;++n) for (int t=0;t<40;++t) {
        WeightedGraph graph(n);
        vector<vector<ll>> matrix(n,vector<ll>(n,INF));
        for (int i=0;i<n;++i) matrix[i][i]=0;
        for (int u=0;u<n;++u) for (int v=0;v<n;++v) {
            int copies=static_cast<int>(rng()%3);
            for (int k=0;k<copies;++k) {
                ll weight=rng()%21;
                graph[u].push_back({v,weight});
                matrix[u][v]=min(matrix[u][v],weight);
            }
        }
        auto distances=floyd(matrix);
        for (int source=0;source<n;++source)
            assert(distances[source]==dijkstra(graph,source));
        ++floyd_cases;
    }

    for (int n=0;n<=40;++n) {
        vector<ll> a(n);
        for (auto& x:a) x=static_cast<int>(rng()%101)-50;
        PrefixSum p(a);
        for (int l=0;l<=n;++l) for (int r=l;r<=n;++r)
            assert(p.sum(l,r)==accumulate(a.begin()+l,a.begin()+r,0LL));
        Difference diff(a);
        for (int t=0;t<100;++t) {
            int l=static_cast<int>(rng()%(n+1)), r=static_cast<int>(rng()%(n+1));
            if (l>r) swap(l,r);
            ll v=static_cast<int>(rng()%21)-10;
            diff.add(l,r,v);
            for (int i=l;i<r;++i) a[i]+=v;
        }
        assert(diff.values()==a);
    }

    auto prime=sieve(100003);
    assert(!is_prime(-1)); assert(!is_prime(0)); assert(!is_prime(1));
    for (int n=0;n<=100003;++n) assert(prime[n]==is_prime(n));
    assert(!sieve(0)[0]); assert(!sieve(1)[1]);
    assert(mod_pow(-2,3,5)==2); assert(mod_pow(8,0,1)==0);
    assert(mod_pow(2,10,1000)==24);
    assert(mod_pow(1000000008LL,2,1000000009LL)==1);
    cout << "All algorithm checks passed (" << stone_cases << " stone cases, "
         << floyd_cases << " Floyd graphs).\n";
}
