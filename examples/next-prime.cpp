#include <iostream>
using namespace std;

bool is_prime(long long n) {
    if (n < 2) return false;
    for (long long d = 2; d <= n / d; ++d)
        if (n % d == 0) return false;
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    long long x;
    if (!(cin >> x)) return 0;
    // Original constraint: 2 <= x <= 100000.
    while (!is_prime(x)) ++x;
    cout << x << '\n';
}
