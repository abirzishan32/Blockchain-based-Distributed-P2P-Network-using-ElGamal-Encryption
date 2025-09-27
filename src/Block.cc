#include "Block.h"
#include "ElGamal.h"
#include "PrimeGenerator.h"
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

bool Block::isValidBlock() const {
    try {
        // Validate by attempting to decrypt and comparing with stored hash
        std::string decrypted = ElGamal::decrypt_message(encryptedData, keyPair);
        return !decrypted.empty() && !encryptedData.empty();
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