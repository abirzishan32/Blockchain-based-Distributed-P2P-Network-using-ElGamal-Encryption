#ifndef BLOCK_H
#define BLOCK_H
#include <string>
#include <sstream>
#include <iomanip>
#include "ElGamal.h"

using namespace std;

class Block {
private:
    int blockNumber;
    int nonce;
    string data;                    // Original data (never transmitted)
    string encryptedData;          // Encrypted data for transmission
    string previousBlockRef;
    KeyPair keyPair;              // PRIVATE - never transmitted
    PublicKey publicKey;          // PUBLIC - safe for transmission
    long long sessionKey;         // PRIVATE - never transmitted
    string publicSessionKeyHash;  // PUBLIC - hash of session key for verification

public:
    // Default constructor for container compatibility
    Block() : blockNumber(0), nonce(0), sessionKey(0) {}
    
    // Main constructor
    Block(int blockNum, const string& blockData, const string& prevRef);

    // Copy constructor and assignment operator
    Block(const Block& other) = default;
    Block& operator=(const Block& other) = default;

    // Getters
    int getBlockNumber() const { return blockNumber; }
    int getNonce() const { return nonce; }
    string getData() const;  // Decrypts data using private key
    string getEncryptedData() const { return encryptedData; }
    string getPreviousBlockRef() const { return previousBlockRef; }
    PublicKey getPublicKey() const { return publicKey; }  // SAFE: only public key
    
    // REMOVED: getKeyPair() - no more private key exposure!

    // Setters for deserialization and mining
    void setNonce(int n) { nonce = n; }
    void setEncryptedData(const string& encrypted) { encryptedData = encrypted; }
    void setPublicKey(const PublicKey& pubKey) { publicKey = pubKey; }
    void setPublicSessionKeyHash(const string& hash) { publicSessionKeyHash = hash; }
    
    // Mining-related methods
    string calculateMiningHash() const;
    bool isMinedValid(int difficulty) const;

    // Validation
    bool isValidBlock() const;

    // Block identifier based on encrypted content
    string getBlockIdentifier() const;

    // SECURE serialization - no private keys transmitted
    string serialize() const;
    static Block deserialize(const string& serialized);
    
private:
    // Generate hash of session key for verification
    string generateSessionKeyHash(long long sessionKey) const;
};

#endif