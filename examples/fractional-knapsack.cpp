#include "../templates/algorithms.hpp"
#include <iomanip>
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    csp::ll capacity;
    if (!(std::cin>>n>>capacity)) return 0;
    std::vector<csp::FractionalItem> items(n);
    for (auto& item:items) std::cin>>item.weight>>item.value;
    auto answer=csp::fractional_knapsack(capacity,std::move(items));
    std::cout<<std::fixed<<std::setprecision(2)<<answer<<'\n';
}
