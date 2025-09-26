#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <string>
#include <vector>
#include <utility>

struct KeyPair {
    long long d;       // private key
    long long e1, e2;  // public key
    long long p;       // prime modulus
};

struct CipherBlock {
    long long c1;
    long long c2;
};

class ElGamal {
private:
    static long long mod_exp(long long base, long long exp, long long mod);
    static long long mod_inverse(long long a, long long m);

public:
    static KeyPair generateKeyPair();
    static KeyPair generateKeyPair(long long p, long long e1, long long d);

    static std::pair<long long, long long> encrypt_char(long long m, long long r, const KeyPair& key);
    static long long decrypt_char(const std::pair<long long, long long>& ciphertext, const KeyPair& key);

    static std::string encrypt_message(const std::string& message, long long r, const KeyPair& key);
    static std::string decrypt_message(const std::string& ciphertext, const KeyPair& key);

    static std::string keyPairToString(const KeyPair& key);
    static KeyPair stringToKeyPair(const std::string& keyStr);
};

#endif
