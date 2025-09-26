#include <omnetpp.h>
#include "Blockchain.h"
#include "FuzzyBFT.h"
#include "ByzantineNode.h"
#include <sstream>
#include <map>
#include <set>
#include <iomanip>
#include <algorithm>

using namespace omnetpp;

class Computer : public cSimpleModule {
private:
    Blockchain blockchain;
    int nodeId;
    NodeType nodeType;
    cMessage *blockTimer;
    int maxBroadcastsPerRound;

    // Fuzzy BFT components
    FuzzyBFT fuzzySystem;
    std::map<int, double> nodeReputations;
    std::map<std::string, int> blockVotes;
    std::map<std::string, int> blockValidations;
    std::map<std::string, std::set<int>> blockVoters;
    int totalNodes;
    double trustThreshold;

    // BFT statistics
    int blocksProposed;
    int blocksAccepted;
    int blocksRejected;
    int byzantineDetected;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void createNewBlock();
    void broadcastNewBlockSequentially(const std::string& blockData);
    void handleBlockProposal(cMessage *msg);
    void handleFuzzyVote(cMessage *msg);
    void displayBlockData(const Block& block, const std::string& action);
    void addBlockToChain(const Block& block);

    // Fuzzy BFT decision making
    double calculateNodeReputation(int nodeId);
    double calculateBlockValidity(const std::string& blockData);
    double calculateNetworkConsensus(const std::string& blockId);
    bool makeFuzzyBFTDecision(int proposerNode, const std::string& blockData, const std::string& blockId);
    void updateNodeReputation(int nodeId, bool positiveAction);

    // Byzantine behaviors
    void executeByzantineBehavior(const std::string& blockData);
    bool shouldBroadcast();

    // Utility methods
    void logFuzzyDecision(int proposerNode, double reputation, double validity, double consensus, double trust, bool decision);
};

Define_Module(Computer);

void Computer::initialize() {
    nodeId = par("nodeId");
    nodeType = (NodeType)par("nodeType").intValue();
    blockTimer = new cMessage("blockTimer");
    maxBroadcastsPerRound = 8;
    totalNodes = getParentModule()->par("numNodes");
    trustThreshold = 0.55;

    // Initialize statistics
    blocksProposed = 0;
    blocksAccepted = 0;
    blocksRejected = 0;
    byzantineDetected = 0;

    // Initialize all node reputations to neutral (0.5)
    for (int i = 0; i < totalNodes; i++) {
        nodeReputations[i] = 0.5;
    }

    // Initialize Byzantine node random generator
    ByzantineNode::initializeRandom();

    // Set node colors based on type for visualization
    std::string color, shape;
    switch(nodeType) {
        case HONEST:
            color = "green";
            shape = "oval";
            break;
        case BYZANTINE_SILENT:
            color = "gray";
            shape = "rect";
            break;
        case BYZANTINE_CORRUPT:
            color = "red";
            shape = "rect";
            break;
        case BYZANTINE_DOUBLE:
            color = "orange";
            shape = "rect";
            break;
        case BYZANTINE_RANDOM:
            color = "purple";
            shape = "rect";
            break;
    }

    getDisplayString().setTagArg("b", 1, color.c_str());
    getDisplayString().setTagArg("s", 0, shape.c_str());

    double initialDelay = uniform(2.0, 8.0) + par("miningInterval").doubleValue();
    scheduleAt(simTime() + initialDelay, blockTimer);

    EV << "Computer " << nodeId << " initialized as " << ByzantineNode::nodeTypeToString(nodeType)
       << " with Fuzzy BFT (trust threshold: " << trustThreshold << ")\n";

    // Print fuzzy rules for the first node only
    if (nodeId == 0) {
        fuzzySystem.printFuzzyRules();
    }
}

void Computer::handleMessage(cMessage *msg) {
    // Handle self-messages (timer)
    if (msg->isSelfMessage()) {
        if (msg == blockTimer) {
            if (ByzantineNode::shouldParticipate(nodeType)) {
                createNewBlock();
            }
            double nextInterval = par("miningInterval").doubleValue() * uniform(0.7, 1.3);
            scheduleAt(simTime() + nextInterval, blockTimer);
        }
        return;
    }

    // Handle incoming messages
    bool messageProcessed = false;
    
    if (strcmp(msg->getName(), "fuzzyBlockProposal") == 0) {
        if (!ByzantineNode::shouldDropMessage(nodeType)) {
            handleBlockProposal(msg);
            messageProcessed = true;
        } else {
            EV << "Node " << nodeId << " (" << ByzantineNode::nodeTypeToString(nodeType)
               << ") dropped block proposal message\n";
            messageProcessed = true;
        }
    }
    else if (strcmp(msg->getName(), "fuzzyVote") == 0) {
        if (!ByzantineNode::shouldDropMessage(nodeType)) {
            handleFuzzyVote(msg);
            messageProcessed = true;
        } else {
            messageProcessed = true;
        }
    }

    // Always delete non-self messages after processing
    if (messageProcessed) {
        delete msg;
    } else {
        EV << "Node " << nodeId << " received unknown message: " << msg->getName() << "\n";
        delete msg;
    }
}

void Computer::createNewBlock() {
    std::stringstream ss;
    ss << "FuzzyBFT_Block_N" << nodeId << "_T" << (int)simTime().dbl() 
       << "_Data[Transaction_" << (blocksProposed + 1) << "]";

    blocksProposed++;

    if (nodeType == HONEST) {
        try {
            // Create and add block to local blockchain
            blockchain.addBlock(ss.str());
            Block* newBlock = blockchain.getLatestBlock();
            
            if (newBlock) {
                // Display the created block data
                displayBlockData(*newBlock, "CREATED");
                
                if (shouldBroadcast()) {
                    broadcastNewBlockSequentially(newBlock->serialize());
                }
            }

            updateNodeReputation(nodeId, true);

            EV << "HONEST Node " << nodeId << " created legitimate block "
               << blockchain.getChainLength() << "\n";
        } catch (const std::exception& e) {
            EV << "Error creating block in node " << nodeId << ": " << e.what() << "\n";
        }
    } else {
        executeByzantineBehavior(ss.str());
        EV << "BYZANTINE Node " << nodeId << " (" << ByzantineNode::nodeTypeToString(nodeType)
           << ") executed malicious behavior\n";
    }
}

// **SEQUENTIAL MESSAGE SENDING - One by One**
void Computer::broadcastNewBlockSequentially(const std::string& blockData) {
    int totalGates = gateSize("port");
    int broadcastCount = 0;
    std::vector<int> selectedGates;

    EV << "\n=== SEQUENTIAL BROADCAST STARTED ===\n"
       << "Node " << nodeId << " sending block sequentially to peers\n";

    // Send messages one by one with delays
    for (int attempts = 0; attempts < totalGates && broadcastCount < maxBroadcastsPerRound; attempts++) {
        int randomGate = intuniform(0, totalGates - 1);

        if (gate("port$o", randomGate)->isConnected() &&
            std::find(selectedGates.begin(), selectedGates.end(), randomGate) == selectedGates.end()) {

            selectedGates.push_back(randomGate);

            // Sequential delay: each message sent after previous one
            double sequentialDelay = broadcastCount * 0.5; // 0.5s interval between sends
            
            // Additional Byzantine delay if applicable
            if (ByzantineNode::shouldDelayMessage(nodeType)) {
                sequentialDelay += uniform(0.1, 0.3);
            }

            cMessage *msg = new cMessage("fuzzyBlockProposal");
            msg->addPar("blockData") = blockData.c_str();
            msg->addPar("proposerNode") = nodeId;
            msg->addPar("proposerReputation") = nodeReputations[nodeId];
            msg->addPar("sendOrder") = broadcastCount; // Track send order

            sendDelayed(msg, sequentialDelay, "port$o", randomGate);
            
            EV << "  → Message " << (broadcastCount + 1) << " scheduled for gate " 
               << randomGate << " (delay: " << sequentialDelay << "s)\n";

            broadcastCount++;
        }
    }

    EV << "Sequential broadcast scheduled: " << broadcastCount << " messages\n"
       << "====================================\n\n";
}

void Computer::handleBlockProposal(cMessage *msg) {
    std::string blockData = msg->par("blockData").stringValue();
    int proposerNode = (int)msg->par("proposerNode").longValue();
    int sendOrder = msg->hasPar("sendOrder") ? (int)msg->par("sendOrder").longValue() : 0;

    EV << "\n=== BLOCK PROPOSAL RECEIVED ===\n"
       << "Node " << nodeId << " received block from Node " << proposerNode 
       << " (send order: " << (sendOrder + 1) << ")\n";

    try {
        Block block = Block::deserialize(blockData);
        std::string blockId = block.getBlockIdentifier();
        
        // Display the received block data
        displayBlockData(block, "RECEIVED");

        // **EXECUTE ALL 6 MAMDANI FUZZY STEPS FOR BFT DECISION**
        bool trustDecision = makeFuzzyBFTDecision(proposerNode, blockData, blockId);

        if (trustDecision) {
            blocksAccepted++;
            
            // Add block to local blockchain immediately upon acceptance
            addBlockToChain(block);
            
            EV << "✓ Node " << nodeId << " ACCEPTED and ADDED block from node " << proposerNode 
               << " via Fuzzy BFT\n";
        } else {
            blocksRejected++;
            if (proposerNode >= 15) { // Byzantine nodes start from index 15
                byzantineDetected++;
            }
            EV << "✗ Node " << nodeId << " REJECTED block from node " << proposerNode 
               << " via Fuzzy BFT\n";
        }

        // Update proposer reputation
        updateNodeReputation(proposerNode, trustDecision);

        // Send fuzzy vote to a subset of network (sequential voting)
        cMessage *voteMsg = new cMessage("fuzzyVote");
        voteMsg->addPar("blockId") = blockId.c_str();
        voteMsg->addPar("trustValue") = trustDecision ? 1.0 : 0.0;
        voteMsg->addPar("voterNode") = nodeId;

        // Send vote to a few random nodes (not all)
        int votesSent = 0;
        for (int i = 0; i < 3 && votesSent < 2; i++) {
            int randomGate = intuniform(0, gateSize("port") - 1);
            if (gate("port$o", randomGate)->isConnected()) {
                double voteDelay = uniform(0.1, 0.3);
                sendDelayed(voteMsg->dup(), voteDelay, "port$o", randomGate);
                votesSent++;
            }
        }
        delete voteMsg;
        
        EV << "Sent " << votesSent << " fuzzy votes\n"
           << "===============================\n\n";

    } catch (const std::exception& e) {
        EV << "Node " << nodeId << " received malformed block from node " << proposerNode
           << " - automatic rejection. Error: " << e.what() << "\n";
        updateNodeReputation(proposerNode, false);
        blocksRejected++;
        byzantineDetected++;
    }
}

// **NEW: Display detailed block data**
void Computer::displayBlockData(const Block& block, const std::string& action) {
    EV << "\n╔═══════════════════════════════════════════════════════════╗\n"
       << "║                    BLOCK " << action << " - NODE " << std::setw(2) << nodeId << "                ║\n"
       << "╠═══════════════════════════════════════════════════════════╣\n"
       << "║ Block Number    : " << std::setw(38) << block.getBlockNumber() << " ║\n"
       << "║ Block Data      : " << std::setw(38) << block.getData().substr(0, 35) << "... ║\n"
       << "║ Block ID        : " << std::setw(38) << block.getBlockIdentifier().substr(0, 35) << "... ║\n"
       << "║ Previous Ref    : " << std::setw(38) << block.getPreviousBlockRef().substr(0, 35) << "... ║\n"
       << "║ Encrypted Data  : " << std::setw(38) << (block.getEncryptedData().empty() ? "None" : "ElGamal Encrypted") << " ║\n"
       << "║ Nonce           : " << std::setw(38) << block.getNonce() << " ║\n"
       << "║ Timestamp       : " << std::setw(38) << simTime().str() << " ║\n";
    
    if (action == "ADDED") {
        EV << "║ Chain Position  : " << std::setw(38) << blockchain.getChainLength() << " ║\n";
    }
    
    EV << "╚═══════════════════════════════════════════════════════════╝\n\n";
}

// **NEW: Add block to blockchain with validation**
void Computer::addBlockToChain(const Block& block) {
    try {
        // Validate block before adding
        if (block.isValidBlock()) {
            blockchain.addBlock(block.getData()); // Add using the data
            displayBlockData(block, "ADDED");
            
            EV << "Block successfully added to blockchain!\n"
               << "New blockchain length: " << blockchain.getChainLength() << "\n";
        } else {
            EV << "Block validation failed - not added to blockchain\n";
        }
    } catch (const std::exception& e) {
        EV << "Error adding block to blockchain: " << e.what() << "\n";
    }
}

void Computer::executeByzantineBehavior(const std::string& blockData) {
    updateNodeReputation(nodeId, false);

    if (ByzantineNode::shouldSendInvalidBlock(nodeType)) {
        std::string corruptedData;

        switch(nodeType) {
            case BYZANTINE_CORRUPT: {
                corruptedData = ByzantineNode::corruptBlockData(blockData, nodeId);
                break;
            }
            case BYZANTINE_DOUBLE: {
                // Send multiple conflicting blocks sequentially
                for (int i = 0; i < 2; i++) {
                    std::string doubleData = ByzantineNode::generateDoubleSpendingBlock(nodeId, i);
                    if (shouldBroadcast()) {
                        // Create fake block and display it
                        try {
                            Block fakeBlock(blockchain.getChainLength(), doubleData, 
                                          blockchain.getLatestBlock()->getBlockIdentifier());
                            displayBlockData(fakeBlock, "BYZANTINE_CREATED");
                            broadcastNewBlockSequentially(fakeBlock.serialize());
                        } catch (...) {
                            EV << "Error creating Byzantine block\n";
                        }
                    }
                }
                return;
            }
            case BYZANTINE_RANDOM: {
                if (uniform(0, 1) < 0.5) {
                    corruptedData = ByzantineNode::corruptBlockData(blockData, nodeId);
                } else {
                    corruptedData = ByzantineNode::generateDoubleSpendingBlock(nodeId, 0);
                }
                break;
            }
            default:
                corruptedData = blockData;
        }

        if (shouldBroadcast()) {
            try {
                Block corruptBlock(blockchain.getChainLength(), corruptedData, 
                                 blockchain.getLatestBlock()->getBlockIdentifier());
                displayBlockData(corruptBlock, "BYZANTINE_CREATED");
                broadcastNewBlockSequentially(corruptBlock.serialize());
            } catch (...) {
                EV << "Error creating corrupted block\n";
            }
        }
    }
}

bool Computer::makeFuzzyBFTDecision(int proposerNode, const std::string& blockData, const std::string& blockId) {
    try {
        double nodeReputation = calculateNodeReputation(proposerNode);
        double blockValidity = calculateBlockValidity(blockData);
        double networkConsensus = calculateNetworkConsensus(blockId);

        if (ByzantineNode::isByzantine(nodeType)) {
            nodeReputation = ByzantineNode::manipulateReputationReport(nodeType, nodeReputation);
            blockValidity = ByzantineNode::getCorruptedValidity(nodeType, blockValidity);
        }

        double trustLevel = fuzzySystem.evaluateNodeTrust(nodeReputation, blockValidity, networkConsensus);
        bool decision = trustLevel >= trustThreshold;

        logFuzzyDecision(proposerNode, nodeReputation, blockValidity, networkConsensus, trustLevel, decision);
        return decision;
    } catch (const std::exception& e) {
        EV << "Error in fuzzy BFT decision for node " << nodeId << ": " << e.what() << "\n";
        return false;
    }
}

void Computer::logFuzzyDecision(int proposerNode, double reputation, double validity, double consensus, double trust, bool decision) {
    EV << "=== FUZZY BFT DECISION (Node " << nodeId << ") ===\n"
       << "  Evaluating block from Node " << proposerNode << "\n"
       << "  Input 1 - Node Reputation: " << std::fixed << std::setprecision(3) << reputation << "\n"
       << "  Input 2 - Block Validity: " << validity << "\n"
       << "  Input 3 - Network Consensus: " << consensus << "\n"
       << "  → Fuzzy Trust Level: " << trust << "\n"
       << "  → Decision: " << (decision ? "ACCEPT" : "REJECT") << " (threshold: " << trustThreshold << ")\n"
       << "=============================================\n";
}

double Computer::calculateNodeReputation(int nodeId) {
    if (nodeReputations.find(nodeId) == nodeReputations.end()) {
        nodeReputations[nodeId] = 0.5;
    }
    return nodeReputations[nodeId];
}

double Computer::calculateBlockValidity(const std::string& blockData) {
    try {
        Block block = Block::deserialize(blockData);

        if (!block.isValidBlock()) return 0.0;

        double validity = 1.0;

        std::string data = block.getData();
        if (data.find("CORRUPT") != std::string::npos) validity *= 0.1;
        if (data.find("MALICIOUS") != std::string::npos) validity *= 0.1;
        if (data.find("FAKE") != std::string::npos) validity *= 0.0;
        if (data.find("DOUBLE_SPEND") != std::string::npos) validity *= 0.0;
        if (data.find("SCRAMBLED") != std::string::npos) validity *= 0.2;

        if (block.getBlockNumber() < 0) validity *= 0.3;
        if (data.empty()) validity *= 0.4;

        return std::max(0.0, std::min(1.0, validity));

    } catch (...) {
        return 0.0;
    }
}

double Computer::calculateNetworkConsensus(const std::string& blockId) {
    if (blockValidations.find(blockId) == blockValidations.end()) {
        return 0.5;
    }

    int positiveVotes = blockVotes[blockId];
    int totalVotes = blockValidations[blockId];

    if (totalVotes == 0) return 0.5;

    double consensus = (double)positiveVotes / totalVotes;
    double confidence = std::min(1.0, (double)totalVotes / (totalNodes * 0.1));
    return consensus * confidence + 0.5 * (1.0 - confidence);
}

void Computer::handleFuzzyVote(cMessage *msg) {
    std::string blockId = msg->par("blockId").stringValue();
    double trustValue = msg->par("trustValue").doubleValue();
    int voterNode = (int)msg->par("voterNode").longValue();

    // Prevent double voting
    if (blockVoters[blockId].find(voterNode) != blockVoters[blockId].end()) {
        return;
    }

    blockVoters[blockId].insert(voterNode);

    // Update vote counts
    if (blockValidations.find(blockId) == blockValidations.end()) {
        blockValidations[blockId] = 0;
        blockVotes[blockId] = 0;
    }

    blockValidations[blockId]++;
    if (trustValue > 0.5) {
        blockVotes[blockId]++;
    }

    EV << "Node " << nodeId << " received vote from node " << voterNode 
       << " for block " << blockId << " (trust: " << trustValue << ")\n";
}

void Computer::updateNodeReputation(int nodeId, bool positiveAction) {
    double currentRep = calculateNodeReputation(nodeId);
    double change = positiveAction ? 0.03 : -0.08;
    nodeReputations[nodeId] = std::max(0.0, std::min(1.0, currentRep + change));
}

bool Computer::shouldBroadcast() {
    double probability = ByzantineNode::isByzantine(nodeType) ? 0.6 : 0.8;
    return uniform(0, 1) < probability;
}

void Computer::finish() {
    cancelAndDelete(blockTimer);

    double avgReputation = 0.0;
    for (auto& pair : nodeReputations) {
        avgReputation += pair.second;
    }
    if (!nodeReputations.empty()) {
        avgReputation /= nodeReputations.size();
    }

    double acceptanceRate = (blocksAccepted + blocksRejected) > 0 ? 
                           (double)blocksAccepted / (blocksAccepted + blocksRejected) : 0.0;

    EV << "\n╔═══════════════════════════════════════════════════════════╗\n"
       << "║              FINAL BLOCKCHAIN STATISTICS - NODE " << std::setw(2) << nodeId << "         ║\n"
       << "╠═══════════════════════════════════════════════════════════╣\n"
       << "║ Node Type           : " << std::setw(38) << ByzantineNode::nodeTypeToString(nodeType) << " ║\n"
       << "║ Blockchain Length   : " << std::setw(38) << blockchain.getChainLength() << " ║\n"
       << "║ Blocks Proposed     : " << std::setw(38) << blocksProposed << " ║\n"
       << "║ Blocks Accepted     : " << std::setw(38) << blocksAccepted << " ║\n"
       << "║ Blocks Rejected     : " << std::setw(38) << blocksRejected << " ║\n"
       << "║ Acceptance Rate     : " << std::setw(35) << (acceptanceRate * 100) << "% ║\n"
       << "║ Byzantine Detected  : " << std::setw(38) << byzantineDetected << " ║\n"
       << "║ Own Reputation      : " << std::setw(38) << nodeReputations[nodeId] << " ║\n"
       << "║ Avg Network Rep.    : " << std::setw(38) << avgReputation << " ║\n"
       << "╚═══════════════════════════════════════════════════════════╝\n";
    
    // Display final blockchain state
    if (blockchain.getChainLength() > 0) {
        EV << "\n=== FINAL BLOCKCHAIN STATE ===\n";
        for (size_t i = 0; i < std::min((size_t)5, blockchain.getChainLength()); i++) {
            Block* block = blockchain.getBlockAt(i);
            if (block) {
                EV << "Block " << i << ": " << block->getData().substr(0, 50) << "...\n";
            }
        }
        if (blockchain.getChainLength() > 5) {
            EV << "... and " << (blockchain.getChainLength() - 5) << " more blocks\n";
        }
        EV << "==============================\n\n";
    }
}