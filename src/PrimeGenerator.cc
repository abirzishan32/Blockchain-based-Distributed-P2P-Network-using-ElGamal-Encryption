#include "PrimeGenerator.h"
#include <chrono>

std::vector<long long> PrimeGenerator::largePrimes;
std::mt19937 PrimeGenerator::rng(std::chrono::steady_clock::now().time_since_epoch().count());

void PrimeGenerator::initializePrimes() {

    largePrimes = {

        982451653LL, 982451707LL, 982451743LL, 982451747LL, 982451749LL,
        982451767LL, 982451773LL, 982451813LL, 982451819LL, 982451821LL,
        

        2147483647LL,   // 2^31 - 1 (Mersenne prime)
        2147483659LL, 2147483693LL, 2147483699LL, 2147483701LL,
        2147483709LL, 2147483713LL, 2147483719LL, 2147483729LL,
        

        4294967311LL,   // Large 32-bit prime
        4294967357LL, 4294967371LL, 4294967377LL, 4294967387LL,
        4294967389LL, 4294967459LL, 4294967477LL, 4294967491LL,
    };
}

long long PrimeGenerator::getRandomPrime() {
    if (largePrimes.empty()) {
        initializePrimes();
    }

    std::uniform_int_distribution<size_t> dist(0, largePrimes.size() - 1);
    return largePrimes[dist(rng)];
}

bool PrimeGenerator::isPrime(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

long long PrimeGenerator::generateRandomInRange(long long min, long long max) {
    std::uniform_int_distribution<long long> dist(min, max);
    return dist(rng);
}
