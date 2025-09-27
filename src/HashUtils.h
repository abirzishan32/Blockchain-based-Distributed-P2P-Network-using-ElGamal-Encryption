#ifndef HASHUTILS_H
#define HASHUTILS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <functional>

class HashUtils {
public:
    // Simple SHA-256 simulation for educational purposes
    // Note: This is NOT cryptographically secure - for simulation only
    static std::string calculateSHA256(const std::string& input);
    
    // Validate hash against difficulty target
    static bool isHashValid(const std::string& hash, int difficulty);
    
    // Generate target string for given difficulty
    static std::string generateTarget(int difficulty);
    
    // Hash rate calculation utilities
    static double calculateHashRate(int attempts, double timeSeconds);
};

#endif