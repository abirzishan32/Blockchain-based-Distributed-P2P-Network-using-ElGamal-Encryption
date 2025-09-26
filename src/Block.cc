#include "Block.h"
#include "ElGamal.h"
#include "PrimeGenerator.h"
#include <sstream>

using namespace std;

Block::Block(int blockNum, const string& blockData, const string& prevRef)
    : blockNumber(blockNum), nonce(0), data(blockData), previousBlockRef(prevRef) {

    // Generate ElGamal key pair for this block
    keyPair = ElGamal::generateKeyPair();

    // Generate random session key for encryption
    sessionKey = PrimeGenerator::generateRandomInRange(100, keyPair.p - 100);

    // Encrypt the block data
    encryptedData = ElGamal::encrypt_message(blockData, sessionKey, keyPair);
}

string Block::getData() const {
    // Decrypt data when requested
    return ElGamal::decrypt_message(encryptedData, keyPair);
}

bool Block::isValidBlock() const {
    try {
        // Validate by attempting to decrypt and comparing with original
        std::string decrypted = ElGamal::decrypt_message(encryptedData, keyPair);
        return !decrypted.empty() && decrypted == data;
    } catch (...) {
        return false;
    }
}

string Block::getBlockIdentifier() const {
    // Create identifier based on encrypted data and block number
    stringstream ss;
    ss << blockNumber << "_" << encryptedData.substr(0, 20); // First 20 chars of encrypted data
    return ss.str();
}

string Block::serialize() const {
    stringstream ss;
    ss << blockNumber << "|"
       << nonce << "|"
       << data << "|"
       << encryptedData << "|"
       << previousBlockRef << "|"
       << ElGamal::keyPairToString(keyPair) << "|"
       << sessionKey;
    return ss.str();
}

Block Block::deserialize(const std::string& serialized) {
    stringstream ss(serialized);
    string item;

    getline(ss, item, '|');
    int blockNum = stoi(item);

    getline(ss, item, '|');
    int blockNonce = stoi(item);

    getline(ss, item, '|');
    string blockData = item;

    getline(ss, item, '|');
    string encrypted = item;

    getline(ss, item, '|');
    string prevRef = item;

    getline(ss, item, '|');
    KeyPair key = ElGamal::stringToKeyPair(item);

    getline(ss, item, '|');
    long long sKey = stoll(item);

    Block block(blockNum, blockData, prevRef);
    block.setNonce(blockNonce);
    block.setEncryptedData(encrypted);
    block.setKeyPair(key);
    block.setSessionKey(sKey);

    return block;
}
