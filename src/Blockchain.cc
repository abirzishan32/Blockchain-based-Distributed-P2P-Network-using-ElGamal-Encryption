#include "Blockchain.h"
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

Blockchain::Blockchain() {
    chain.push_back(make_unique<Block>(createGenesisBlock()));
}

Block Blockchain::createGenesisBlock() {
    return Block(0, "Genesis Block - Fuzzy BFT Blockchain Network", "0");
}

void Blockchain::addBlock(const string& data) {
    Block* previousBlock = getLatestBlock();
    Block newBlock(chain.size(), data, previousBlock->getBlockIdentifier());
    chain.push_back(make_unique<Block>(newBlock));
}

// NEW: Add existing block
void Blockchain::addBlock(const Block& block) {
    chain.push_back(make_unique<Block>(block));
}

Block* Blockchain::getLatestBlock() {
    return chain.back().get();
}

// NEW: Get block by index
Block* Blockchain::getBlockAt(size_t index) {
    if (index >= chain.size()) return nullptr;
    return chain[index].get();
}

bool Blockchain::isChainValid() const {
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& currentBlock = *chain[i];
        const Block& previousBlock = *chain[i-1];

        if (!currentBlock.isValidBlock()) {
            return false;
        }

        if (currentBlock.getPreviousBlockRef() != previousBlock.getBlockIdentifier()) {
            return false;
        }
    }
    return true;
}

// NEW: Display entire blockchain
void Blockchain::displayChain() const {
    cout << "\n╔══════════════════════════════════════════╗\n";
    cout << "║            BLOCKCHAIN STATE              ║\n";
    cout << "╠══════════════════════════════════════════╣\n";
    cout << "║ Total Blocks: " << setw(26) << chain.size() << " ║\n";
    cout << "╠══════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < chain.size(); i++) {
        const Block& block = *chain[i];
        cout << "║ Block " << setw(2) << i << ": " << setw(29) << block.getData().substr(0, 29) << " ║\n";
        cout << "║   ID: " << setw(33) << block.getBlockIdentifier().substr(0, 33) << " ║\n";
        if (i < chain.size() - 1) {
            cout << "╠──────────────────────────────────────────╣\n";
        }
    }
    
    cout << "╚══════════════════════════════════════════╝\n\n";
}

vector<Block> Blockchain::getChain() const {
    vector<Block> result;
    for (const auto& block : chain) {
        result.push_back(*block);
    }
    return result;
}

bool Blockchain::replaceChain(const vector<Block>& newChain) {
    if (newChain.size() <= chain.size()) {
        return false;
    }

    // Validate new chain
    for (size_t i = 1; i < newChain.size(); i++) {
        if (!newChain[i].isValidBlock()) {
            return false;
        }
        if (i > 0 && newChain[i].getPreviousBlockRef() != newChain[i-1].getBlockIdentifier()) {
            return false;
        }
    }

    // Replace chain
    chain.clear();
    for (const Block& block : newChain) {
        chain.push_back(make_unique<Block>(block));
    }

    return true;
}

string Blockchain::serialize() const {
    string result;
    for (size_t i = 0; i < chain.size(); i++) {
        result += chain[i]->serialize();
        if (i < chain.size() - 1) {
            result += ";";
        }
    }
    return result;
}

void Blockchain::deserialize(const string& serialized) {
    chain.clear();

    stringstream ss(serialized);
    string blockStr;

    while (getline(ss, blockStr, ';')) {
        Block block = Block::deserialize(blockStr);
        chain.push_back(make_unique<Block>(block));
    }
}