#include "Block.h"
#include "SHA256.h"
#include <sstream>

Block::Block(int blockNum, const std::string& blockData, const std::string& prevHash)
    : blockNumber(blockNum), nonce(0), data(blockData), previousHash(prevHash) {
    hash = calculateHash();
}

std::string Block::calculateHash() const {
    std::stringstream ss;
    ss << blockNumber << nonce << data << previousHash;

    // Use proper SHA256 hash function
    return SHA256::hash(ss.str());
}

bool Block::isValidBlock() const {
    return hash == calculateHash();
}

std::string Block::serialize() const {
    std::stringstream ss;
    ss << blockNumber << "|" << nonce << "|" << data << "|" << previousHash << "|" << hash;
    return ss.str();
}

Block Block::deserialize(const std::string& serialized) {
    std::stringstream ss(serialized);
    std::string item;

    std::getline(ss, item, '|');
    int blockNum = std::stoi(item);

    std::getline(ss, item, '|');
    int blockNonce = std::stoi(item);

    std::getline(ss, item, '|');
    std::string blockData = item;

    std::getline(ss, item, '|');
    std::string prevHash = item;

    std::getline(ss, item, '|');
    std::string blockHash = item;

    Block block(blockNum, blockData, prevHash);
    block.setNonce(blockNonce);
    block.setHash(blockHash);

    return block;
}
