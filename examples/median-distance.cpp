#include "../templates/algorithms.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    if (!(std::cin>>n)) return 0;
    std::vector<csp::ll> positions(n);
    for (auto& x:positions) std::cin>>x;
    std::cout<<csp::min_absolute_distance_sum(std::move(positions))<<'\n';
}
