#ifndef COMPUTER_H
#define COMPUTER_H

#include <omnetpp.h>
#include "Blockchain.h"
#include "FuzzyBFT.h"
#include "ByzantineNode.h"
#include "MiningEngine.h"
#include <map>
#include <set>

using namespace omnetpp;
using namespace std;

class Computer : public cSimpleModule {
private:
    Blockchain blockchain;
    int nodeId;
    NodeType nodeType;
    cMessage *blockTimer;
    int maxBroadcastsPerRound;

    // Fuzzy BFT components
    FuzzyBFT fuzzySystem;
    map<int, double> nodeReputations;
    map<string, int> blockVotes;
    map<string, int> blockValidations;
    map<string, set<int>> blockVoters;
    int totalNodes;
    double trustThreshold;

    // Mining components
    MiningEngine miningEngine;
    int miningDifficulty;
    bool miningEnabled;

    // BFT statistics
    int blocksProposed;
    int blocksAccepted;
    int blocksRejected;
    int byzantineDetected;
    
    // Mining statistics
    int blocksMined;
    double totalMiningTime;
    int totalMiningAttempts;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    // Mining-enabled block creation
    void createNewBlock();
    void mineAndBroadcastBlock(const std::string& blockData);
    
    void broadcastNewBlockSequentially(const std::string& blockData);
    void handleBlockProposal(cMessage *msg);
    void handleFuzzyVote(cMessage *msg);
    void displayBlockData(const Block& block, const std::string& action);
    void addBlockToChain(const Block& block);

    // Enhanced validation with mining verification
    double calculateBlockValidity(const std::string& blockData);
    
    // Fuzzy BFT decision making
    double calculateNodeReputation(int nodeId);
    double calculateNetworkConsensus(const std::string& blockId);
    bool makeFuzzyBFTDecision(int proposerNode, const std::string& blockData, const std::string& blockId);
    void updateNodeReputation(int nodeId, bool positiveAction);

    // Byzantine behaviors
    void executeByzantineBehavior(const std::string& blockData);
    bool shouldBroadcast();

    // Utility methods
    void logFuzzyDecision(int proposerNode, double reputation, double validity, 
                         double consensus, double trust, bool decision);
                         
    // Mining statistics display
    void displayMiningStats();
};

#endif