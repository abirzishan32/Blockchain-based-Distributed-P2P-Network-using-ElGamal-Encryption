#include "ByzantineNode.h"
#include <chrono>
#include <sstream>

std::mt19937 ByzantineNode::rng(std::chrono::steady_clock::now().time_since_epoch().count());

void ByzantineNode::initializeRandom() {
    rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
}

std::string ByzantineNode::corruptBlockData(const std::string& originalData, int nodeId) {
    std::stringstream ss;

    std::uniform_int_distribution<int> corruptionType(1, 4);

    switch(corruptionType(rng)) {
        case 1:
            // Corrupt prefix
            ss << "CORRUPT_" << originalData;
            break;
        case 2:
            // Inject malicious data
            ss << originalData << "_MALICIOUS_" << nodeId;
            break;
        case 3:
            // Replace with fake data
            ss << "FAKE_BLOCK_NODE_" << nodeId << "_" << std::time(nullptr);
            break;
        case 4:
            // Scramble original data
            ss << "SCRAMBLED_" << originalData.substr(originalData.length()/2)
               << "_" << originalData.substr(0, originalData.length()/2);
            break;
        default:
            ss << "BYZANTINE_" << originalData;
    }

    return ss.str();
}

bool ByzantineNode::shouldSendInvalidBlock(NodeType type) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_CORRUPT:
            return dist(rng) < 0.8; // 80% chance to send invalid blocks
        case BYZANTINE_DOUBLE:
            return dist(rng) < 0.6; // 60% chance to send conflicting blocks
        case BYZANTINE_RANDOM:
            return dist(rng) < 0.5; // 50% random behavior
        case BYZANTINE_SILENT:
        case HONEST:
        default:
            return false;
    }
}

bool ByzantineNode::shouldParticipate(NodeType type) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_SILENT:
            return dist(rng) < 0.2; // 20% participation rate
        case BYZANTINE_CORRUPT:
        case BYZANTINE_DOUBLE:
        case BYZANTINE_RANDOM:
            return dist(rng) < 0.7; // 70% participation to stay hidden
        case HONEST:
        default:
            return true;
    }
}

double ByzantineNode::getCorruptedValidity(NodeType type, double originalValidity) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_CORRUPT:
            // Corrupt nodes report low validity for good blocks, high for bad blocks
            return originalValidity > 0.5 ? dist(rng) * 0.4 : 0.6 + dist(rng) * 0.4;

        case BYZANTINE_DOUBLE:
            // Double-spending nodes always report high validity
            return 0.8 + dist(rng) * 0.2;

        case BYZANTINE_RANDOM:
            // Random validity reports
            return dist(rng);

        case BYZANTINE_SILENT:
            // Silent nodes don't report
            return 0.0;

        case HONEST:
        default:
            return originalValidity;
    }
}

std::string ByzantineNode::generateDoubleSpendingBlock(int nodeId, int sequence) {
    std::stringstream ss;
    ss << "DOUBLE_SPEND_NODE_" << nodeId << "_SEQ_" << sequence
       << "_TIME_" << std::time(nullptr) << "_AMOUNT_999999";
    return ss.str();
}

double ByzantineNode::manipulateReputationReport(NodeType type, double actualReputation) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_CORRUPT:
            // Report higher reputation for Byzantine nodes, lower for honest nodes
            return actualReputation < 0.5 ? 0.7 + dist(rng) * 0.3 : dist(rng) * 0.4;

        case BYZANTINE_DOUBLE:
            // Always report high reputation
            return 0.8 + dist(rng) * 0.2;

        case BYZANTINE_RANDOM:
            // Random reputation reports
            return dist(rng);

        default:
            return actualReputation;
    }
}

bool ByzantineNode::shouldDropMessage(NodeType type) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_SILENT:
            return dist(rng) < 0.3; // 30% message drop rate
        case BYZANTINE_CORRUPT:
            return dist(rng) < 0.1; // 10% message drop rate
        case BYZANTINE_RANDOM:
            return dist(rng) < 0.2; // 20% message drop rate
        default:
            return false;
    }
}

bool ByzantineNode::shouldDelayMessage(NodeType type) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    switch(type) {
        case BYZANTINE_CORRUPT:
        case BYZANTINE_DOUBLE:
            return dist(rng) < 0.15; // 15% delay rate
        case BYZANTINE_RANDOM:
            return dist(rng) < 0.25; // 25% delay rate
        default:
            return false;
    }
}

std::string ByzantineNode::nodeTypeToString(NodeType type) {
    switch(type) {
        case HONEST: return "HONEST";
        case BYZANTINE_SILENT: return "BYZANTINE_SILENT";
        case BYZANTINE_CORRUPT: return "BYZANTINE_CORRUPT";
        case BYZANTINE_DOUBLE: return "BYZANTINE_DOUBLE";
        case BYZANTINE_RANDOM: return "BYZANTINE_RANDOM";
        default: return "UNKNOWN";
    }
}

bool ByzantineNode::isByzantine(NodeType type) {
    return type != HONEST;
}
