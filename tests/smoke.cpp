#include "../templates/algorithms.hpp"
#include <cassert>
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

    // All possible switch positions, including all-false and all-true.
    for (int n=0;n<60;++n)
        for (int cut=0;cut<=n;++cut)
            assert(first_true(0,n,[&](ll x){return x>=cut;})==cut);

    assert(max_nonoverlap({})==0);
    assert(max_nonoverlap({{1,3},{2,4},{3,5},{-2,1}})==3);
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
    cout << "All algorithm checks passed.\n";
}
