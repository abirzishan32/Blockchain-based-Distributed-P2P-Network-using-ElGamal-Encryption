#include "Blockchain.h"
#include <sstream>

Blockchain::Blockchain() {
    chain.push_back(std::make_unique<Block>(createGenesisBlock()));
}

Block Blockchain::createGenesisBlock() {
    return Block(0, "Genesis Block", "0");
}

void Blockchain::addBlock(const std::string& data) {
    Block* previousBlock = getLatestBlock();
    Block newBlock(chain.size(), data, previousBlock->getHash());

    chain.push_back(std::make_unique<Block>(newBlock));
}

Block* Blockchain::getLatestBlock() {
    return chain.back().get();
}

bool Blockchain::isChainValid() const {
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& currentBlock = *chain[i];
        const Block& previousBlock = *chain[i-1];

        if (!currentBlock.isValidBlock()) {
            return false;
        }

        if (currentBlock.getPreviousHash() != previousBlock.getHash()) {
            return false;
        }
    }
    return true;
}

std::vector<Block> Blockchain::getChain() const {
    std::vector<Block> result;
    for (const auto& block : chain) {
        result.push_back(*block);
    }
    return result;
}

bool Blockchain::replaceChain(const std::vector<Block>& newChain) {
    if (newChain.size() <= chain.size()) {
        return false;
    }

    // Validate new chain
    for (size_t i = 1; i < newChain.size(); i++) {
        if (!newChain[i].isValidBlock()) {
            return false;
        }
        if (newChain[i].getPreviousHash() != newChain[i-1].getHash()) {
            return false;
        }
    }

    // Replace chain
    chain.clear();
    for (const Block& block : newChain) {
        chain.push_back(std::make_unique<Block>(block));
    }

    return true;
}

std::string Blockchain::serialize() const {
    std::string result;
    for (size_t i = 0; i < chain.size(); i++) {
        result += chain[i]->serialize();
        if (i < chain.size() - 1) {
            result += ";";
        }
    }
    return result;
}

void Blockchain::deserialize(const std::string& serialized) {
    chain.clear();

    std::stringstream ss(serialized);
    std::string blockStr;

    while (std::getline(ss, blockStr, ';')) {
        Block block = Block::deserialize(blockStr);
        chain.push_back(std::make_unique<Block>(block));
    }
}
