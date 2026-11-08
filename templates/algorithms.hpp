#pragma once
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <limits>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// C++17. Vertex indices and array indices are 0-based.
// Copy only the functions needed by a problem, with their required headers.
namespace csp {
using ll = long long;
using Graph = std::vector<std::vector<int>>;
constexpr ll INF = std::numeric_limits<ll>::max() / 4;

// Unweighted shortest path. g must be nonempty; 0 <= source < g.size().
// Unreachable vertices have distance -1. O(V+E) time, O(V) extra space.
inline std::vector<int> bfs(const Graph& g, int source) {
    std::vector<int> dist(g.size(), -1);
    std::queue<int> q;
    dist[source] = 0;
    q.push(source);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : g[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1; // Mark when enqueuing.
            q.push(v);
        }
    }
    return dist;
}

// Reachability using an explicit stack; NOT recursive postorder.
// g nonempty, source valid. O(V+E) time, O(V) extra space.
inline std::vector<int> dfs_order(const Graph& g, int source) {
    std::vector<int> order;
    std::vector<bool> seen(g.size(), false);
    std::stack<int> st;
    seen[source] = true; st.push(source);
    while (!st.empty()) {
        int u = st.top(); st.pop();
        order.push_back(u);
        for (int v : g[u]) {
            if (seen[v]) continue;
            seen[v] = true; st.push(v);
        }
    }
    return order;
}

// Four-neighbor components of '1' cells in a rectangular binary grid.
// Empty grid returns 0. O(rows*cols) time and extra space.
inline int grid_components(const std::vector<std::string>& grid) {
    if (grid.empty()) return 0;
    int n = static_cast<int>(grid.size()), m = static_cast<int>(grid[0].size());
    std::vector<std::vector<bool>> seen(n, std::vector<bool>(m, false));
    const int dx[] = {1,-1,0,0}, dy[] = {0,0,1,-1};
    int count = 0;
    for (int i=0; i<n; ++i) for (int j=0; j<m; ++j) {
        if (grid[i][j]!='1' || seen[i][j]) continue;
        ++count;
        std::stack<std::pair<int,int>> st;
        seen[i][j] = true; st.push({i,j});
        while (!st.empty()) {
            auto [x,y] = st.top(); st.pop();
            for (int k=0; k<4; ++k) {
                int nx=x+dx[k], ny=y+dy[k];
                if (nx<0 || nx>=n || ny<0 || ny>=m) continue;
                if (grid[nx][ny]!='1' || seen[nx][ny]) continue;
                seen[nx][ny] = true; st.push({nx,ny});
            }
        }
    }
    return count;
}

// Nonempty rectangular grid: '.', '#', exactly one S and T.
// Other uppercase letters occur in pairs. Entering a portal and teleporting
// is ONE move; arrival at its exit does not trigger another teleport.
// Returns -1 if unreachable. O(rows*cols) time and extra space.
inline int grid_portal_distance(const std::vector<std::string>& grid) {
    int n = static_cast<int>(grid.size()), m = static_cast<int>(grid[0].size());
    std::pair<int,int> start{-1,-1}, target{-1,-1};
    std::vector<std::vector<std::pair<int,int>>> portals(26);
    for (int i=0; i<n; ++i) for (int j=0; j<m; ++j) {
        char c=grid[i][j];
        if (c=='S') start={i,j};
        else if (c=='T') target={i,j};
        else if ('A'<=c && c<='Z') portals[c-'A'].push_back({i,j});
    }
    std::vector<std::vector<int>> dist(n, std::vector<int>(m,-1));
    std::queue<std::pair<int,int>> q;
    dist[start.first][start.second]=0; q.push(start);
    const int dx[] = {1,-1,0,0}, dy[] = {0,0,1,-1};
    while (!q.empty()) {
        auto [x,y]=q.front(); q.pop();
        for (int k=0; k<4; ++k) {
            int nx=x+dx[k], ny=y+dy[k];
            if (nx<0 || nx>=n || ny<0 || ny>=m || grid[nx][ny]=='#') continue;
            char c=grid[nx][ny];
            if ('A'<=c && c<='Z' && c!='S' && c!='T') {
                const auto& p=portals[c-'A'];
                auto exit=(p[0]==std::make_pair(nx,ny) ? p[1] : p[0]);
                nx=exit.first; ny=exit.second;
            }
            if (dist[nx][ny]!=-1) continue;
            dist[nx][ny]=dist[x][y]+1; q.push({nx,ny});
        }
    }
    return dist[target.first][target.second];
}

// Predicate on [lo, hi) must be false...false,true...true.
// Returns the original hi when no true exists, including an empty interval.
// Require 0 <= lo <= hi <= LLONG_MAX. O(log(hi-lo+1)) predicate calls.
template<class Predicate>
ll first_true(ll lo, ll hi, Predicate ok) {
    while (lo < hi) {
        ll mid = lo + (hi - lo) / 2;
        if (ok(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}

// Predicate on CLOSED [lo,hi] must be true...true,false...false; ok(lo) is true.
// Require 0 <= lo <= hi <= LLONG_MAX. O(log(hi-lo+1)) predicate calls.
template<class Predicate>
ll last_true(ll lo, ll hi, Predicate ok) {
    while (lo<hi) {
        ll mid=hi-(hi-lo)/2; // Upper midpoint, without overflowing hi-lo+1.
        if (ok(mid)) lo=mid;
        else hi=mid-1;
    }
    return lo;
}

// Endpoints 0 and length cannot be removed. Require length>=1,
// distinct stones strictly inside (0,length), 0<=max_removed<=stones.size().
// O(n log n + n log(length+1)) time, O(n) input-copy space.
inline ll max_min_stone_gap(ll length, std::vector<ll> stones, int max_removed) {
    std::sort(stones.begin(), stones.end());
    auto feasible=[&](ll gap) {
        int removed=0;
        ll last=0;
        for (ll x:stones) {
            if (x-last<gap) ++removed;
            else last=x;
        }
        // If the last segment is too short, remove the last kept interior stone.
        // Only the count is needed; neither endpoint is actually removed.
        if (length-last<gap) ++removed;
        return removed<=max_removed;
    };
    return last_true(0,length,feasible);
}

// Maximum count of pairwise nonoverlapping half-open intervals [l,r).
// Require l < r. Touching endpoints are compatible. O(n log n).
inline int max_nonoverlap(std::vector<std::pair<ll,ll>> intervals) {
    std::sort(intervals.begin(), intervals.end(), [](auto a, auto b) {
        return a.second != b.second ? a.second < b.second : a.first < b.first;
    });
    ll end = std::numeric_limits<ll>::lowest();
    int count = 0;
    for (auto [l,r] : intervals) {
        if (l >= end) { ++count; end = r; }
    }
    return count;
}

struct FractionalItem { int weight, value; };
// Divisible items, capacity>=0, 1<=weight<=1e9, 0<=value<=1e9.
// Exact density ordering by safe long long products; floating-point total.
// O(n log n) time, O(n) input-copy space.
inline long double fractional_knapsack(ll capacity, std::vector<FractionalItem> items) {
    std::sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
        return 1LL*a.value*b.weight > 1LL*b.value*a.weight;
    });
    long double answer=0;
    for (auto item:items) {
        ll take=std::min(capacity,1LL*item.weight);
        answer+=static_cast<long double>(item.value)*take/item.weight;
        capacity-=take;
        if (capacity==0) break;
    }
    return answer;
}

// Minimize sum |x-p| by choosing a median. Empty input returns 0.
// Pairwise differences and the total must fit long long.
// O(n log n) time, O(n) input-copy space.
inline ll min_absolute_distance_sum(std::vector<ll> positions) {
    if (positions.empty()) return 0;
    std::sort(positions.begin(), positions.end());
    ll median=positions[positions.size()/2], total=0;
    for (ll x:positions) total+=std::abs(x-median);
    return total;
}

class DSU {
    std::vector<int> parent, size_;
public:
    explicit DSU(int n) : parent(n), size_(n,1) {
        std::iota(parent.begin(), parent.end(), 0);
    }
    int find(int x) {
        while (x != parent[x]) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (size_[a] < size_[b]) std::swap(a,b);
        parent[b] = a; size_[a] += size_[b];
        return true;
    }
    bool same(int a, int b) { return find(a) == find(b); }
};

using WeightedGraph = std::vector<std::vector<std::pair<int,ll>>>;
// Nonnegative edge weights only; g nonempty and source valid.
// Every finite shortest distance must be < INF. Unreachable returns INF.
// Heap version O((V+E)log(V+E)) time, O(V+E) space including heap.
inline std::vector<ll> dijkstra(const WeightedGraph& g, int source) {
    for (const auto& edges : g)
        for (auto [v,w] : edges) {
            (void)v;
            if (w < 0) throw std::invalid_argument("Dijkstra requires nonnegative weights");
        }
    std::vector<ll> dist(g.size(), INF);
    using State = std::pair<ll,int>;
    std::priority_queue<State,std::vector<State>,std::greater<State>> pq;
    dist[source] = 0; pq.push({0,source});
    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        for (auto [v,w] : g[u]) {
            if (w >= INF || d >= INF-w) continue;
            if (d+w < dist[v]) {
                dist[v] = d+w; pq.push({dist[v],v});
            }
        }
    }
    return dist;
}

// Square matrix: diagonal 0, absent edges INF, parallel edges minimized.
// Finite entries must be in [0,INF); finite shortest distances must be < INF.
// O(V^3) time and O(V^2) matrix space (the parameter is passed by value).
inline std::vector<std::vector<ll>> floyd(std::vector<std::vector<ll>> dist) {
    int n=static_cast<int>(dist.size());
    for (int k=0; k<n; ++k)
        for (int i=0; i<n; ++i)
            for (int j=0; j<n; ++j)
                if (dist[i][k]!=INF && dist[k][j]!=INF)
                    dist[i][j]=std::min(dist[i][j],dist[i][k]+dist[k][j]);
    return dist;
}

// Directed graph. If returned size < V, graph contains a directed cycle.
// O(V+E) time, O(V) extra space. The empty graph returns an empty order.
inline std::vector<int> topo_sort(const Graph& g) {
    std::vector<int> indegree(g.size(),0), order;
    for (const auto& edges : g) for (int v : edges) ++indegree[v];
    std::queue<int> q;
    for (int u=0; u<static_cast<int>(g.size()); ++u)
        if (indegree[u]==0) q.push(u);
    while (!q.empty()) {
        int u=q.front(); q.pop(); order.push_back(u);
        for (int v : g[u]) if (--indegree[v]==0) q.push(v);
    }
    return order;
}

// Static range sums on [l,r). Require 0 <= l <= r <= a.size().
// Build O(n); each query O(1); O(n) space. Sums must fit long long.
class PrefixSum {
    std::vector<ll> prefix;
public:
    explicit PrefixSum(const std::vector<ll>& a) : prefix(a.size()+1,0) {
        for (std::size_t i=0; i<a.size(); ++i) prefix[i+1]=prefix[i]+a[i];
    }
    ll sum(int l,int r) const { return prefix[r]-prefix[l]; }
};

// Offline range addition on [l,r), then materialize once.
// add O(1); values O(n). Base and update sums must fit long long.
class Difference {
    std::vector<ll> diff;
public:
    explicit Difference(const std::vector<ll>& a) : diff(a.size()+1,0) {
        for (std::size_t i=0; i<a.size(); ++i)
            diff[i]=a[i]-(i ? a[i-1] : 0);
    }
    void add(int l,int r,ll value) {
        diff[l]+=value; diff[r]-=value;
    }
    std::vector<ll> values() const {
        std::vector<ll> a(diff.size()-1);
        ll current=0;
        for (std::size_t i=0; i<a.size(); ++i) {
            current+=diff[i]; a[i]=current;
        }
        return a;
    }
};

// Trial division O(sqrt(n)), O(1) space. Intended for modest n.
inline bool is_prime(ll n) {
    if (n<2) return false;
    if (n%2==0) return n==2;
    for (ll d=3; d<=n/d; d+=2) if (n%d==0) return false;
    return true;
}

// Eratosthenes sieve on [0,n], n >= 0.
// O(n log log n) time for n >= 3; O(n) space.
inline std::vector<bool> sieve(int n) {
    std::vector<bool> prime(static_cast<std::size_t>(n)+1,true);
    prime[0]=false;
    if (n>=1) prime[1]=false;
    for (int p=2; p<=n/p; ++p)
        if (prime[p])
            for (ll j=1LL*p*p; j<=n; j+=p) prime[static_cast<std::size_t>(j)]=false;
    return prime;
}

// Require exponent >= 0 and 1 <= mod <= 1,000,000,009.
// The modulus bound makes long long multiplication safe. O(log exponent).
inline ll mod_pow(ll base,ll exponent,ll mod) {
    base%=mod;
    if (base<0) base+=mod;
    ll result=1%mod;
    while (exponent>0) {
        if (exponent&1) result=result*base%mod;
        base=base*base%mod;
        exponent>>=1;
    }
    return result;
}

// Require n<=20 and every subset sum to fit long long. Includes the empty sum 0.
// K distinct sums: O(2^n * (n+log(K+1))) time, O(K) result space.
inline std::set<ll> distinct_subset_sums(const std::vector<ll>& a) {
    int n=static_cast<int>(a.size());
    std::set<ll> sums;
    for (unsigned long long mask=0; mask<(1ULL<<n); ++mask) {
        ll sum=0;
        for (int i=0; i<n; ++i) if ((mask>>i)&1ULL) sum+=a[i];
        sums.insert(sum);
    }
    return sums;
}
} // namespace csp
