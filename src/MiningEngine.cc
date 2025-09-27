#include "MiningEngine.h"
#include "HashUtils.h"
#include "ElGamal.h"
#include <omnetpp.h>
#include <sstream>
#include <iostream>
#include <chrono>

using namespace omnetpp;

MiningEngine::MiningEngine(int diff) {
    difficulty = diff;
    maxAttempts = 100000; // Limit for simulation
    showProgress = true;
    progressInterval = 5000; // Show progress every 5k attempts
}

MiningResult MiningEngine::mineBlock(Block& block) {
    MiningResult result;
    result.success = false;
    result.goldenNonce = 0;
    result.attempts = 0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    EV << "🔨 Mining block " << block.getBlockNumber() 
       << " with difficulty " << difficulty << "...\n";
    
    std::string target = HashUtils::generateTarget(difficulty);
    EV << "🎯 Target: " << target.substr(0, 20) << "...\n";
    
    // Mining loop - find golden nonce
    for (int nonce = 0; nonce <= maxAttempts; nonce++) {
        result.attempts++;
        
        // Calculate hash with current nonce
        std::string blockHash = calculateBlockHash(block, nonce);
        
        // Check if hash meets difficulty target
        if (HashUtils::isHashValid(blockHash, difficulty)) {
            // Golden nonce found!
            result.success = true;
            result.goldenNonce = nonce;
            result.blockHash = blockHash;
            
            // Update block with golden nonce
            block.setNonce(nonce);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            result.miningTimeMs = duration.count();
            result.hashRate = HashUtils::calculateHashRate(result.attempts, result.miningTimeMs / 1000.0);
            
            EV << "⛏️  GOLDEN NONCE FOUND!\n"
               << "   Nonce: " << result.goldenNonce << "\n"
               << "   Hash: " << result.blockHash << "\n"
               << "   Attempts: " << result.attempts << "\n"
               << "   Mining time: " << result.miningTimeMs << " ms\n"
               << "   Hash rate: " << std::fixed << std::setprecision(2) 
               << result.hashRate << " H/s\n";
            
            return result;
        }
        
        // Show progress
        if (showProgress && nonce % progressInterval == 0 && nonce > 0) {
            EV << "   ⚙️ Attempt " << nonce << ": hash " 
               << blockHash.substr(0, 10) << "... (not valid)\n";
        }
    }
    
    // Mining failed - no golden nonce found within limit
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.miningTimeMs = duration.count();
    result.hashRate = HashUtils::calculateHashRate(result.attempts, result.miningTimeMs / 1000.0);
    
    EV << "❌ Mining failed! No golden nonce found within " 
       << maxAttempts << " attempts\n";
    
    return result;
}

std::string MiningEngine::calculateBlockHash(const Block& block, int nonce) {
    std::stringstream ss;
    
    // Construct block data for hashing - same as Block::calculateMiningHash but with custom nonce
    ss << block.getBlockNumber() << "|"
       << block.getEncryptedData() << "|"
       << block.getPreviousBlockRef() << "|"
       << ElGamal::publicKeyToString(block.getPublicKey()) << "|"
       << nonce; // This is the mining variable!
    
    return HashUtils::calculateSHA256(ss.str());
}

bool MiningEngine::validateMinedBlock(const Block& block) {
    // Recalculate hash with block's nonce
    std::string recalculatedHash = calculateBlockHash(block, block.getNonce());
    
    // Verify hash meets difficulty requirement
    return HashUtils::isHashValid(recalculatedHash, difficulty);
}