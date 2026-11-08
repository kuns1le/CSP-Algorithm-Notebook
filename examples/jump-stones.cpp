#include "../templates/algorithms.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    csp::ll length;
    int n,m;
    if (!(std::cin>>length>>n>>m)) return 0;
    std::vector<csp::ll> stones(n);
    for (auto& x:stones) std::cin>>x;
    std::cout<<csp::max_min_stone_gap(length,std::move(stones),m)<<'\n';
}
