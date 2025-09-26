#ifndef PRIMEGENERATOR_H
#define PRIMEGENERATOR_H

#include <vector>
#include <random>

class PrimeGenerator {
private:
    static std::vector<long long> largePrimes;
    static std::mt19937 rng;

public:
    static void initializePrimes();
    static long long getRandomPrime();
    static bool isPrime(long long n);
    static long long generateRandomInRange(long long min, long long max);
};

#endif
