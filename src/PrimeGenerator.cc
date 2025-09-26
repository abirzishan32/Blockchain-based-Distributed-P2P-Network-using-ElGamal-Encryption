#include "PrimeGenerator.h"
#include <chrono>

std::vector<long long> PrimeGenerator::largePrimes;
std::mt19937 PrimeGenerator::rng(std::chrono::steady_clock::now().time_since_epoch().count());

void PrimeGenerator::initializePrimes() {
    // List of large prime numbers suitable for ElGamal
    largePrimes = {
        30803, 30809, 30817, 30829, 30839, 30841, 30851, 30853,
        30859, 30869, 30871, 30881, 30893, 30911, 30931, 30937,
        30941, 30949, 30971, 30977, 30983, 31013, 31019, 31033,
        31039, 31051, 31063, 31069, 31079, 31081, 31091, 31121,
        31123, 31139, 31147, 31151, 31153, 31159, 31177, 31181,
        65519, 65521, 65537, 65539, 65543, 65551, 65557, 65563,
        65579, 65581, 65587, 65599, 65609, 65617, 65629, 65633,
        982451653, 982451707, 982451743, 982451747, 982451749,
        982451767, 982451773, 982451813, 982451819, 982451821
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
