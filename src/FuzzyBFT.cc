#include "FuzzyBFT.h"
#include "FuzzyMembership.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

FuzzyBFT::FuzzyBFT() {
    initializeFuzzyRules();
}

// STEP 1: Determining a set of fuzzy rules
void FuzzyBFT::initializeFuzzyRules() {
    rules.clear();

    // High Trust Rules (Byzantine Fault Tolerant)
    rules.push_back({HIGH, HIGH, HIGH, HIGH, 1.0,
        "IF reputation=HIGH AND validity=HIGH AND consensus=HIGH THEN trust=HIGH"});

    rules.push_back({HIGH, HIGH, MEDIUM, HIGH, 0.9,
        "IF reputation=HIGH AND validity=HIGH AND consensus=MEDIUM THEN trust=HIGH"});

    rules.push_back({HIGH, MEDIUM, HIGH, HIGH, 0.8,
        "IF reputation=HIGH AND validity=MEDIUM AND consensus=HIGH THEN trust=HIGH"});

    // Medium Trust Rules (Cautious acceptance)
    rules.push_back({MEDIUM, HIGH, HIGH, MEDIUM, 0.7,
        "IF reputation=MEDIUM AND validity=HIGH AND consensus=HIGH THEN trust=MEDIUM"});

    rules.push_back({HIGH, MEDIUM, MEDIUM, MEDIUM, 0.6,
        "IF reputation=HIGH AND validity=MEDIUM AND consensus=MEDIUM THEN trust=MEDIUM"});

    rules.push_back({MEDIUM, MEDIUM, HIGH, MEDIUM, 0.5,
        "IF reputation=MEDIUM AND validity=MEDIUM AND consensus=HIGH THEN trust=MEDIUM"});

    rules.push_back({MEDIUM, HIGH, MEDIUM, MEDIUM, 0.5,
        "IF reputation=MEDIUM AND validity=HIGH AND consensus=MEDIUM THEN trust=MEDIUM"});

    // Low Trust Rules (Byzantine detection and rejection)
    rules.push_back({LOW, HIGH, HIGH, LOW, 0.8,
        "IF reputation=LOW AND validity=HIGH AND consensus=HIGH THEN trust=LOW"});

    rules.push_back({HIGH, LOW, HIGH, LOW, 0.9,
        "IF reputation=HIGH AND validity=LOW AND consensus=HIGH THEN trust=LOW"});

    rules.push_back({HIGH, HIGH, LOW, LOW, 0.8,
        "IF reputation=HIGH AND validity=HIGH AND consensus=LOW THEN trust=LOW"});

    rules.push_back({LOW, LOW, HIGH, LOW, 1.0,
        "IF reputation=LOW AND validity=LOW AND consensus=HIGH THEN trust=LOW"});

    rules.push_back({LOW, HIGH, LOW, LOW, 1.0,
        "IF reputation=LOW AND validity=HIGH AND consensus=LOW THEN trust=LOW"});

    rules.push_back({HIGH, LOW, LOW, LOW, 1.0,
        "IF reputation=HIGH AND validity=LOW AND consensus=LOW THEN trust=LOW"});

    // Critical Byzantine Rules (Strong rejection)
    rules.push_back({LOW, LOW, LOW, LOW, 1.0,
        "IF reputation=LOW AND validity=LOW AND consensus=LOW THEN trust=LOW"});

    rules.push_back({LOW, MEDIUM, LOW, LOW, 0.9,
        "IF reputation=LOW AND validity=MEDIUM AND consensus=LOW THEN trust=LOW"});

    rules.push_back({MEDIUM, LOW, MEDIUM, LOW, 0.8,
        "IF reputation=MEDIUM AND validity=LOW AND consensus=MEDIUM THEN trust=LOW"});

    // Mixed scenarios
    rules.push_back({MEDIUM, MEDIUM, MEDIUM, MEDIUM, 0.4,
        "IF reputation=MEDIUM AND validity=MEDIUM AND consensus=MEDIUM THEN trust=MEDIUM"});

    std::cout << "Fuzzy BFT System initialized with " << rules.size() << " rules\n";
}

// **STEP 2: Fuzzifying the inputs using membership functions**
std::map<FuzzySet, double> FuzzyBFT::fuzzifyNodeReputation(double reputation) {
    std::map<FuzzySet, double> fuzzy;

    // Node Reputation membership functions (0.0 to 1.0)
    fuzzy[LOW] = FuzzyMembership::trapezoidalMembership(reputation, 0.0, 0.0, 0.2, 0.4);
    fuzzy[MEDIUM] = FuzzyMembership::triangularMembership(reputation, 0.2, 0.5, 0.8);
    fuzzy[HIGH] = FuzzyMembership::trapezoidalMembership(reputation, 0.6, 0.8, 1.0, 1.0);

    return fuzzy;
}

std::map<FuzzySet, double> FuzzyBFT::fuzzifyBlockValidity(double validity) {
    std::map<FuzzySet, double> fuzzy;

    // Block Validity membership functions (0.0 to 1.0)
    fuzzy[LOW] = FuzzyMembership::trapezoidalMembership(validity, 0.0, 0.0, 0.3, 0.5);
    fuzzy[MEDIUM] = FuzzyMembership::triangularMembership(validity, 0.3, 0.6, 0.8);
    fuzzy[HIGH] = FuzzyMembership::trapezoidalMembership(validity, 0.7, 0.85, 1.0, 1.0);

    return fuzzy;
}

std::map<FuzzySet, double> FuzzyBFT::fuzzifyNetworkConsensus(double consensus) {
    std::map<FuzzySet, double> fuzzy;

    // Network Consensus membership functions (0.0 to 1.0)
    // LOW: 0-40% consensus (Byzantine attack scenario)
    fuzzy[LOW] = FuzzyMembership::trapezoidalMembership(consensus, 0.0, 0.0, 0.25, 0.4);

    // MEDIUM: 30-70% consensus (uncertain scenario)
    fuzzy[MEDIUM] = FuzzyMembership::triangularMembership(consensus, 0.3, 0.5, 0.7);

    // HIGH: 60-100% consensus (BFT safety threshold)
    fuzzy[HIGH] = FuzzyMembership::trapezoidalMembership(consensus, 0.6, 0.75, 1.0, 1.0);

    return fuzzy;
}

// **STEP 3: Combining fuzzified inputs according to fuzzy rules**
double FuzzyBFT::calculateRuleStrength(const FuzzyRule& rule,
                                      const std::map<FuzzySet, double>& repFuzzy,
                                      const std::map<FuzzySet, double>& validFuzzy,
                                      const std::map<FuzzySet, double>& consFuzzy) {

    // Mamdani AND operation using MIN operator
    double minValue = std::min({
        repFuzzy.at(rule.nodeReputation),
        validFuzzy.at(rule.blockValidity),
        consFuzzy.at(rule.networkConsensus)
    });

    // Apply rule weight
    return minValue * rule.weight;
}

// **STEP 4: Finding consequence by combining rule strength and output membership**
std::map<FuzzySet, double> FuzzyBFT::calculateConsequence(double ruleStrength, FuzzySet outputSet) {
    std::map<FuzzySet, double> consequence;
    consequence[LOW] = 0.0;
    consequence[MEDIUM] = 0.0;
    consequence[HIGH] = 0.0;

    // Mamdani clipping method: min(rule_strength, membership_function)
    consequence[outputSet] = ruleStrength;

    return consequence;
}

// **Main function executing all 6 Mamdani steps**
double FuzzyBFT::evaluateNodeTrust(double nodeReputation, double blockValidity, double networkConsensus) {
    // **STEP 2: Fuzzify all inputs**
    auto repFuzzy = fuzzifyNodeReputation(nodeReputation);
    auto validFuzzy = fuzzifyBlockValidity(blockValidity);
    auto consFuzzy = fuzzifyNetworkConsensus(networkConsensus);

    // **STEP 5: Combining consequences to get output distribution**
    std::map<FuzzySet, double> outputDistribution;
    outputDistribution[LOW] = 0.0;
    outputDistribution[MEDIUM] = 0.0;
    outputDistribution[HIGH] = 0.0;

    // Process each rule
    for (const auto& rule : rules) {
        // **STEP 3: Calculate rule strength**
        double ruleStrength = calculateRuleStrength(rule, repFuzzy, validFuzzy, consFuzzy);

        if (ruleStrength > 0.001) { // Only process rules with significant strength
            // **STEP 4: Calculate consequence**
            auto consequence = calculateConsequence(ruleStrength, rule.trustLevel);

            // **STEP 5: Aggregate using MAX operator (Mamdani)**
            for (auto& pair : consequence) {
                outputDistribution[pair.first] = std::max(outputDistribution[pair.first], pair.second);
            }
        }
    }

    // **STEP 6: Defuzzify output distribution**
    return defuzzifyCoG(outputDistribution);
}

// **STEP 6: Defuzzifying the output distribution using Center of Gravity**
double FuzzyBFT::defuzzifyCoG(const std::map<FuzzySet, double>& outputDistribution) {
    // Center of Gravity (Centroid) method
    double numerator = 0.0;
    double denominator = 0.0;

    // Centroid values for trust levels
    double lowCentroid = 0.15;    // Low trust
    double mediumCentroid = 0.5;  // Medium trust
    double highCentroid = 0.85;   // High trust

    numerator += outputDistribution.at(LOW) * lowCentroid;
    numerator += outputDistribution.at(MEDIUM) * mediumCentroid;
    numerator += outputDistribution.at(HIGH) * highCentroid;

    denominator += outputDistribution.at(LOW);
    denominator += outputDistribution.at(MEDIUM);
    denominator += outputDistribution.at(HIGH);

    if (denominator < 0.001) {
        return 0.5; // Default neutral trust
    }

    return numerator / denominator;
}

// Alternative defuzzification: Mean of Maximum
double FuzzyBFT::defuzzifyMOM(const std::map<FuzzySet, double>& outputDistribution) {
    double maxMembership = std::max({
        outputDistribution.at(LOW),
        outputDistribution.at(MEDIUM),
        outputDistribution.at(HIGH)
    });

    if (maxMembership < 0.001) return 0.5;

    // Find which set(s) have maximum membership
    if (outputDistribution.at(HIGH) == maxMembership) return 0.85;
    if (outputDistribution.at(MEDIUM) == maxMembership) return 0.5;
    if (outputDistribution.at(LOW) == maxMembership) return 0.15;

    return 0.5;
}

// Utility methods
void FuzzyBFT::printFuzzyRules() {
    std::cout << "\n=== FUZZY BFT RULES ===\n";
    for (size_t i = 0; i < rules.size(); i++) {
        std::cout << "Rule " << (i+1) << ": " << rules[i].description
                  << " (weight: " << rules[i].weight << ")\n";
    }
    std::cout << "=======================\n\n";
}

std::string FuzzyBFT::fuzzySetToString(FuzzySet set) {
    switch(set) {
        case LOW: return "LOW";
        case MEDIUM: return "MEDIUM";
        case HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}
