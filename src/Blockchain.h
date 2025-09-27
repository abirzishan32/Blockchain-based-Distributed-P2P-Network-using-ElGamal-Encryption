#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "Block.h"
#include <vector>
#include <memory>
#include <sstream>

using namespace std;

class Blockchain {
private:
    vector<unique_ptr<Block>> chain;
    Block createGenesisBlock();

public:
    Blockchain();

    void addBlock(const string& data);
    void addBlock(const Block& block);  // NEW: Add existing block
    Block* getLatestBlock();
    Block* getBlockAt(size_t index);   // NEW: Get block by index
    bool isChainValid() const;

    // Network synchronization methods
    vector<Block> getChain() const;
    bool replaceChain(const vector<Block>& newChain);

    // Getters
    size_t getChainLength() const { return chain.size(); }

    // Display methods
    void displayChain() const;  // NEW: Display entire chain

    // Serialization for network transmission
    string serialize() const;
    void deserialize(const string& serialized);
};

#endif