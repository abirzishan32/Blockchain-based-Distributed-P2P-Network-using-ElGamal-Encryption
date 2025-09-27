#include "HashUtils.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstring>

// Simple SHA-256 simulation for educational purposes
// Note: This is NOT cryptographically secure - for simulation only
std::string HashUtils::calculateSHA256(const std::string& input) {
    // Use multiple hash functions to create a more realistic distribution
    std::hash<std::string> hasher1;
    std::hash<std::string> hasher2;
    
    // Create compound hash from input
    size_t hash1 = hasher1(input);
    size_t hash2 = hasher2(input + "salt");
    
    // Combine hashes to create 64-character hex string (simulating SHA-256)
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << hash1 << std::setw(16) << hash2;
    
    // Pad to 64 characters to simulate SHA-256 output
    std::string result = ss.str();
    while (result.length() < 64) {
        result = "0" + result;
    }
    
    // Truncate to 64 characters if too long
    if (result.length() > 64) {
        result = result.substr(0, 64);
    }
    
    return result;
}

bool HashUtils::isHashValid(const std::string& hash, int difficulty) {
    if (hash.length() < difficulty) return false;
    
    // Check if hash starts with required number of zeros
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            return false;
        }
    }
    
    return true;
}

std::string HashUtils::generateTarget(int difficulty) {
    return std::string(difficulty, '0') + std::string(64 - difficulty, 'f');
}

double HashUtils::calculateHashRate(int attempts, double timeSeconds) {
    if (timeSeconds <= 0) return 0.0;
    return attempts / timeSeconds; // Hashes per second
}