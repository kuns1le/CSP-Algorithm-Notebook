#include "../templates/algorithms.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n,m;
    if (!(std::cin>>n>>m)) return 0;
    std::vector<std::string> grid(n);
    for (auto& row:grid) std::cin>>row;
    std::cout<<csp::grid_components(grid)<<'\n';
}
