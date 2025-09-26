#ifndef BYZANTINENODE_H
#define BYZANTINENODE_H

#include <string>
#include <random>

using namespace std;

enum NodeType {
    HONEST = 0,
    BYZANTINE_SILENT = 1,    // Silent/Fail-stop Byzantine
    BYZANTINE_CORRUPT = 2,   // Sends corrupted data
    BYZANTINE_DOUBLE = 3,    // Double-spending/conflicting messages
    BYZANTINE_RANDOM = 4     // Random Byzantine behavior
};

class ByzantineNode {
private:
    static mt19937 rng;

public:
    static void initializeRandom();

    // Byzantine behavior implementations
    static string corruptBlockData(const string& originalData, int nodeId);
    static bool shouldSendInvalidBlock(NodeType type);
    static bool shouldParticipate(NodeType type);
    static double getCorruptedValidity(NodeType type, double originalValidity);
    static string generateDoubleSpendingBlock(int nodeId, int sequence);

    // Reputation manipulation
    static double manipulateReputationReport(NodeType type, double actualReputation);

    // Network disruption
    static bool shouldDropMessage(NodeType type);
    static bool shouldDelayMessage(NodeType type);

    // Utility functions
    static std::string nodeTypeToString(NodeType type);
    static bool isByzantine(NodeType type);
};

#endif
