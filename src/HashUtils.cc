#include "HashUtils.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstring>

using namespace std;

string HashUtils::calculateSHA256(const string& input) {

    hash<string> hasher1;
    hash<string> hasher2;


    size_t hash1 = hasher1(input);
    size_t hash2 = hasher2(input + "salt");
    

        stringstream ss;
    ss << hex << setfill('0') << setw(16) << hash1 << setw(16) << hash2;

    // Pad to 64 characters to simulate SHA-256 output
    string result = ss.str();
    while (result.length() < 64) {
        result = "0" + result;
    }
    
    // Truncate to 64 characters if too long
    if (result.length() > 64) {
        result = result.substr(0, 64);
    }
    
    return result;
}

bool HashUtils::isHashValid(const string& hash, int difficulty) {
    if (hash.length() < difficulty) return false;
    

    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != '0') {
            return false;
        }
    }
    
    return true;
}

string HashUtils::generateTarget(int difficulty) {
    return string(difficulty, '0') + string(64 - difficulty, 'f');
}

double HashUtils::calculateHashRate(int attempts, double timeSeconds) {
    if (timeSeconds <= 0) return 0.0;
    return attempts / timeSeconds; 
}