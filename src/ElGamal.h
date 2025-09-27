#ifndef ELGAMAL_H
#define ELGAMAL_H

#include <string>
#include <vector>
#include <utility>

struct KeyPair {
    long long d;       // private key - NEVER TRANSMITTED
    long long e1, e2;  // public key components
    long long p;       // prime modulus
};

struct PublicKey {
    long long e1, e2;  // public key components only
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
    
    // Extract public key from key pair (SAFE for transmission)
    static PublicKey extractPublicKey(const KeyPair& keyPair);

    static std::pair<long long, long long> encrypt_char(long long m, long long r, const PublicKey& publicKey);
    static long long decrypt_char(const std::pair<long long, long long>& ciphertext, const KeyPair& keyPair);

    static std::string encrypt_message(const std::string& message, long long r, const PublicKey& publicKey);
    static std::string decrypt_message(const std::string& ciphertext, const KeyPair& keyPair);

    // SECURE serialization - only public key components
    static std::string publicKeyToString(const PublicKey& publicKey);
    static PublicKey stringToPublicKey(const std::string& keyStr);
    

};

#endif