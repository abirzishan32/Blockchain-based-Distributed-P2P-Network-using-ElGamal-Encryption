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
    string data;
    string encryptedData;
    string previousBlockRef;
    KeyPair keyPair;
    long long sessionKey;

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
    string getData() const;
    string getEncryptedData() const { return encryptedData; }
    string getPreviousBlockRef() const { return previousBlockRef; }
    KeyPair getKeyPair() const { return keyPair; }

    // Setters for deserialization
    void setNonce(int n) { nonce = n; }
    void setEncryptedData(const string& encrypted) { encryptedData = encrypted; }
    void setKeyPair(const KeyPair& key) { keyPair = key; }
    void setSessionKey(long long key) { sessionKey = key; }

    // Validation
    bool isValidBlock() const;

    // Block identifier based on encrypted content
    string getBlockIdentifier() const;

    // Serialization for network transmission
    string serialize() const;
    static Block deserialize(const string& serialized);
};

#endif