#include "../templates/algorithms.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n,m,q;
    if (!(std::cin>>n>>m>>q)) return 0;
    std::vector<std::vector<csp::ll>> d(n,std::vector<csp::ll>(n,csp::INF));
    for (int i=0; i<n; ++i) d[i][i]=0;
    for (int i=0; i<m; ++i) {
        int u,v;
        csp::ll w;
        std::cin>>u>>v>>w;
        --u; --v;
        d[u][v]=d[v][u]=std::min(d[u][v],w);
    }
    d=csp::floyd(std::move(d));
    while (q--) {
        int u,v;
        std::cin>>u>>v;
        --u; --v;
        std::cout<<(d[u][v]==csp::INF ? -1 : d[u][v])<<'\n';
    }
}
