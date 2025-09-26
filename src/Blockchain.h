#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "Block.h"
#include <vector>
#include <memory>
#include <sstream>

class Blockchain {
private:
    std::vector<std::unique_ptr<Block>> chain;

    Block createGenesisBlock();

public:
    Blockchain();

    void addBlock(const std::string& data);
    Block* getLatestBlock();
    bool isChainValid() const;

    // Network synchronization methods
    std::vector<Block> getChain() const;
    bool replaceChain(const std::vector<Block>& newChain);

    // Getters
    size_t getChainLength() const { return chain.size(); }

    // Serialization for network transmission
    std::string serialize() const;
    void deserialize(const std::string& serialized);
};

#endif
