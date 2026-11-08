#include "../templates/algorithms.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    if (!(std::cin>>n)) return 0;
    std::vector<csp::ll> a(n);
    for (auto& x:a) std::cin>>x;
    std::cout<<csp::distinct_subset_sums(a).size()<<'\n';
}
