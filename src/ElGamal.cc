#include "ElGamal.h"
#include "PrimeGenerator.h"
#include <sstream>
#include <iostream>

long long ElGamal::mod_exp(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) res = (__int128)res * base % mod;
        base = (__int128)base * base % mod;
        exp >>= 1;
    }
    return res;
}

long long ElGamal::mod_inverse(long long a, long long m) {
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    if (x1 < 0) x1 += m0;
    return x1;
}

KeyPair ElGamal::generateKeyPair() {
    long long p = PrimeGenerator::getRandomPrime();
    long long e1 = 2; // Simple generator
    long long d = PrimeGenerator::generateRandomInRange(2, p - 2);

    return generateKeyPair(p, e1, d);
}

KeyPair ElGamal::generateKeyPair(long long p, long long e1, long long d) {
    KeyPair key;
    key.p = p;
    key.e1 = e1;
    key.d = d;
    key.e2 = mod_exp(e1, d, p);
    return key;
}

std::pair<long long, long long> ElGamal::encrypt_char(long long m, long long r, const KeyPair& key) {
    long long c1 = mod_exp(key.e1, r, key.p);
    long long c2 = (m * mod_exp(key.e2, r, key.p)) % key.p;
    return {c1, c2};
}

long long ElGamal::decrypt_char(const std::pair<long long, long long>& ciphertext, const KeyPair& key) {
    long long c1 = ciphertext.first;
    long long c2 = ciphertext.second;
    long long c1d = mod_exp(c1, key.d, key.p);
    long long inv = mod_inverse(c1d, key.p);
    return (c2 * inv) % key.p;
}

std::string ElGamal::encrypt_message(const std::string& message, long long r, const KeyPair& key) {
    std::ostringstream oss;
    for (size_t i = 0; i < message.length(); i++) {
        long long m = (unsigned char)message[i];
        auto cipher = encrypt_char(m, r + i, key); // Use different r for each char
        oss << cipher.first << "," << cipher.second;
        if (i < message.length() - 1) oss << ";";
    }
    return oss.str();
}

std::string ElGamal::decrypt_message(const std::string& ciphertext, const KeyPair& key) {
    std::stringstream ss(ciphertext);
    std::string block;
    std::string result;

    while (std::getline(ss, block, ';')) {
        if (block.empty()) continue;
        size_t pos = block.find(",");
        if (pos == std::string::npos) continue;

        long long c1 = std::stoll(block.substr(0, pos));
        long long c2 = std::stoll(block.substr(pos + 1));
        long long m = decrypt_char({c1, c2}, key);
        result.push_back((char)m);
    }
    return result;
}

std::string ElGamal::keyPairToString(const KeyPair& key) {
    std::ostringstream oss;
    oss << key.d << ":" << key.e1 << ":" << key.e2 << ":" << key.p;
    return oss.str();
}

KeyPair ElGamal::stringToKeyPair(const std::string& keyStr) {
    std::stringstream ss(keyStr);
    std::string item;
    KeyPair key;

    std::getline(ss, item, ':');
    key.d = std::stoll(item);

    std::getline(ss, item, ':');
    key.e1 = std::stoll(item);

    std::getline(ss, item, ':');
    key.e2 = std::stoll(item);

    std::getline(ss, item, ':');
    key.p = std::stoll(item);

    return key;
}
