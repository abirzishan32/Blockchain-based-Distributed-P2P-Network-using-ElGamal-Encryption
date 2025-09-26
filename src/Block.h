#ifndef BLOCK_H
#define BLOCK_H
#include <string>
#include <sstream>
#include <iomanip>

class Block {
private:
    int blockNumber;
    int nonce;
    std::string data;
    std::string previousHash;
    std::string hash;

    std::string calculateHash() const;

public:
    Block(int blockNum, const std::string& blockData, const std::string& prevHash);

    // Add copy constructor and assignment operator
    Block(const Block& other) = default;
    Block& operator=(const Block& other) = default;

    // Getters
    int getBlockNumber() const { return blockNumber; }
    int getNonce() const { return nonce; }
    std::string getData() const { return data; }
    std::string getPreviousHash() const { return previousHash; }
    std::string getHash() const { return hash; }

    // Setters for deserialization
    void setNonce(int n) { nonce = n; }
    void setHash(const std::string& h) { hash = h; }

    // Validation
    bool isValidBlock() const;

    // Serialization for network transmission
    std::string serialize() const;
    static Block deserialize(const std::string& serialized);
};

#endif
