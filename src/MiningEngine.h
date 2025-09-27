#ifndef MININGENGINE_H
#define MININGENGINE_H

#include "Block.h"
#include <string>
#include <chrono>

struct MiningResult {
    bool success;
    int goldenNonce;
    std::string blockHash;
    int attempts;
    double miningTimeMs;
    double hashRate;
};

class MiningEngine {
private:
    int difficulty;
    int maxAttempts;
    bool showProgress;
    int progressInterval;

public:
    MiningEngine(int diff = 4);
    
    // Main mining function
    MiningResult mineBlock(Block& block);
    
    // Configuration methods
    void setDifficulty(int diff) { difficulty = diff; }
    void setMaxAttempts(int maxAtt) { maxAttempts = maxAtt; }
    void setShowProgress(bool show) { showProgress = show; }
    
    // Utility methods
    std::string calculateBlockHash(const Block& block, int nonce);
    bool validateMinedBlock(const Block& block);
};

#endif