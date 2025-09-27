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

// NEW: Extract only public components
PublicKey ElGamal::extractPublicKey(const KeyPair& keyPair) {
    PublicKey pubKey;
    pubKey.e1 = keyPair.e1;
    pubKey.e2 = keyPair.e2;
    pubKey.p = keyPair.p;
    return pubKey;
}

// UPDATED: Use PublicKey for encryption (no private key needed)
std::pair<long long, long long> ElGamal::encrypt_char(long long m, long long r, const PublicKey& publicKey) {
    long long c1 = mod_exp(publicKey.e1, r, publicKey.p);
    long long c2 = (m * mod_exp(publicKey.e2, r, publicKey.p)) % publicKey.p;
    return {c1, c2};
}

// Decryption still uses private key (only locally)
long long ElGamal::decrypt_char(const std::pair<long long, long long>& ciphertext, const KeyPair& keyPair) {
    long long c1 = ciphertext.first;
    long long c2 = ciphertext.second;
    long long c1d = mod_exp(c1, keyPair.d, keyPair.p);
    long long inv = mod_inverse(c1d, keyPair.p);
    return (c2 * inv) % keyPair.p;
}

// UPDATED: Encryption uses PublicKey only
std::string ElGamal::encrypt_message(const std::string& message, long long r, const PublicKey& publicKey) {
    std::ostringstream oss;
    for (size_t i = 0; i < message.length(); i++) {
        long long m = (unsigned char)message[i];
        auto cipher = encrypt_char(m, r + i, publicKey); // Use different r for each char
        oss << cipher.first << "," << cipher.second;
        if (i < message.length() - 1) oss << ";";
    }
    return oss.str();
}

// Decryption uses full KeyPair (private key)
std::string ElGamal::decrypt_message(const std::string& ciphertext, const KeyPair& keyPair) {
    std::stringstream ss(ciphertext);
    std::string block;
    std::string result;

    while (std::getline(ss, block, ';')) {
        if (block.empty()) continue;
        size_t pos = block.find(",");
        if (pos == std::string::npos) continue;

        long long c1 = std::stoll(block.substr(0, pos));
        long long c2 = std::stoll(block.substr(pos + 1));
        long long m = decrypt_char({c1, c2}, keyPair);
        result.push_back((char)m);
    }
    return result;
}

// NEW: SECURE serialization - public key only
std::string ElGamal::publicKeyToString(const PublicKey& publicKey) {
    std::ostringstream oss;
    oss << publicKey.e1 << ":" << publicKey.e2 << ":" << publicKey.p;
    // NO PRIVATE KEY IN TRANSMISSION!
    return oss.str();
}

// NEW: SECURE deserialization - public key only
PublicKey ElGamal::stringToPublicKey(const std::string& keyStr) {
    std::stringstream ss(keyStr);
    std::string item;
    PublicKey publicKey;

    std::getline(ss, item, ':');
    publicKey.e1 = std::stoll(item);

    std::getline(ss, item, ':');
    publicKey.e2 = std::stoll(item);

    std::getline(ss, item, ':');
    publicKey.p = std::stoll(item);

    return publicKey;
}

