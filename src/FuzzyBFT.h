#ifndef FUZZYBFT_H
#define FUZZYBFT_H

#include <vector>
#include <map>
#include <string>

enum FuzzySet {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2
};

struct FuzzyRule {
    FuzzySet nodeReputation;    // Input 1
    FuzzySet blockValidity;     // Input 2
    FuzzySet networkConsensus;  // Input 3
    FuzzySet trustLevel;        // Output
    double weight;              // Rule weight
    std::string description;    // Rule description
};

class FuzzyBFT {
private:
    std::vector<FuzzyRule> rules;

    // Step 3: Rule strength calculation methods
    double calculateRuleStrength(const FuzzyRule& rule,
                               const std::map<FuzzySet, double>& repFuzzy,
                               const std::map<FuzzySet, double>& validFuzzy,
                               const std::map<FuzzySet, double>& consFuzzy);

    // Step 4: Consequence calculation
    std::map<FuzzySet, double> calculateConsequence(double ruleStrength, FuzzySet outputSet);

    // Step 6: Defuzzification methods
    double defuzzifyCoG(const std::map<FuzzySet, double>& outputDistribution);
    double defuzzifyMOM(const std::map<FuzzySet, double>& outputDistribution);

public:
    FuzzyBFT();

    // Step 1: Initialize fuzzy rules
    void initializeFuzzyRules();

    // Step 2: Fuzzification methods
    std::map<FuzzySet, double> fuzzifyNodeReputation(double reputation);
    std::map<FuzzySet, double> fuzzifyBlockValidity(double validity);
    std::map<FuzzySet, double> fuzzifyNetworkConsensus(double consensus);

    // Main BFT decision function (executes all 6 steps)
    double evaluateNodeTrust(double nodeReputation, double blockValidity, double networkConsensus);

    // Utility methods
    void printFuzzyRules();
    std::string fuzzySetToString(FuzzySet set);
};

#endif
