#include "Block.h"
#include "ElGamal.h"
#include "PrimeGenerator.h"
#include "HashUtils.h"
#include <sstream>
#include <iomanip>
#include <functional>

using namespace std;

Block::Block(int blockNum, const string& blockData, const string& prevRef)
    : blockNumber(blockNum), nonce(0), data(blockData), previousBlockRef(prevRef) {

    // Generate ElGamal key pair for this block
    keyPair = ElGamal::generateKeyPair();
    
    // Extract public key (safe for transmission)
    publicKey = ElGamal::extractPublicKey(keyPair);

    // Generate random session key for encryption
    sessionKey = PrimeGenerator::generateRandomInRange(100, keyPair.p - 100);
    
    // Generate public hash of session key (for verification without exposing key)
    publicSessionKeyHash = generateSessionKeyHash(sessionKey);

    // Encrypt the block data using PUBLIC KEY ONLY
    encryptedData = ElGamal::encrypt_message(blockData, sessionKey, publicKey);
}

string Block::getData() const {
    // Decrypt data when requested using PRIVATE KEY
    return ElGamal::decrypt_message(encryptedData, keyPair);
}

// Calculate hash for mining purposes
string Block::calculateMiningHash() const {
    stringstream ss;
    ss << blockNumber << "|"
       << encryptedData << "|"
       << previousBlockRef << "|"
       << ElGamal::publicKeyToString(publicKey) << "|"
       << nonce;  // Nonce affects the hash!
    
    return HashUtils::calculateSHA256(ss.str());
}

// Validate if block was properly mined
bool Block::isMinedValid(int difficulty) const {
    string blockHash = calculateMiningHash();
    return HashUtils::isHashValid(blockHash, difficulty);
}

bool Block::isValidBlock() const {
    try {
        // Validate structure and encryption
        std::string decrypted = ElGamal::decrypt_message(encryptedData, keyPair);
        bool structurallyValid = !decrypted.empty() && !encryptedData.empty();
        
        // Also validate mining (if nonce > 0, assume it was mined)
        bool miningValid = (nonce == 0) || isMinedValid(4); // Default difficulty 4
        
        return structurallyValid && miningValid;
    } catch (...) {
        return false;
    }
}

string Block::getBlockIdentifier() const {
    // Include nonce in identifier for mined blocks
    stringstream ss;
    ss << blockNumber << "_" << nonce << "_" << encryptedData.substr(0, 20);
    return ss.str();
}

// SECURE serialization - NO PRIVATE KEYS!
string Block::serialize() const {
    stringstream ss;
    ss << blockNumber << "|"
       << nonce << "|"
       // REMOVED: << data << "|"                    // NO PLAINTEXT DATA!
       << encryptedData << "|"                        // Encrypted data only
       << previousBlockRef << "|"
       << ElGamal::publicKeyToString(publicKey) << "|"  // PUBLIC KEY ONLY!
       << publicSessionKeyHash;                       // SESSION KEY HASH ONLY!
       // REMOVED: << sessionKey;                    // NO SESSION KEY!
    return ss.str();
}

Block Block::deserialize(const std::string& serialized) {
    stringstream ss(serialized);
    string item;

    getline(ss, item, '|');
    int blockNum = stoi(item);

    getline(ss, item, '|');
    int blockNonce = stoi(item);

    // REMOVED: plaintext data reading
    getline(ss, item, '|');
    string encrypted = item;

    getline(ss, item, '|');
    string prevRef = item;

    getline(ss, item, '|');
    PublicKey pubKey = ElGamal::stringToPublicKey(item);  // PUBLIC KEY ONLY

    getline(ss, item, '|');
    string sessionKeyHash = item;

    // NOTE: We cannot fully reconstruct the block without private key and session key
    // This is intentional - remote nodes can only see public data
    Block block;
    block.blockNumber = blockNum;
    block.nonce = blockNonce;
    block.encryptedData = encrypted;
    block.previousBlockRef = prevRef;
    block.publicKey = pubKey;
    block.publicSessionKeyHash = sessionKeyHash;

    return block;
}

string Block::generateSessionKeyHash(long long sessionKey) const {
    // Simple hash function for session key verification
    hash<string> hasher;
    stringstream ss;
    ss << sessionKey << "_" << blockNumber << "_salt";
    return to_string(hasher(ss.str()));
}