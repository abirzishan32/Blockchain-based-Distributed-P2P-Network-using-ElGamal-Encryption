#ifndef FUZZYBFT_H
#define FUZZYBFT_H

#include <vector>
#include <map>
#include <string>

using namespace std; 

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
    vector<FuzzyRule> rules;

    // Step 3: Rule strength calculation methods
    double calculateRuleStrength(const FuzzyRule& rule,
                               const map<FuzzySet, double>& repFuzzy,
                               const map<FuzzySet, double>& validFuzzy,
                               const map<FuzzySet, double>& consFuzzy);

    // Step 4: Consequence calculation
    map<FuzzySet, double> calculateConsequence(double ruleStrength, FuzzySet outputSet);

    // Step 6: Defuzzification methods
    double defuzzifyCoG(const map<FuzzySet, double>& outputDistribution);
    double defuzzifyMOM(const map<FuzzySet, double>& outputDistribution);

public:
    FuzzyBFT();

    // Step 1: Initialize fuzzy rules
    void initializeFuzzyRules();

    // Step 2: Fuzzification methods
    map<FuzzySet, double> fuzzifyNodeReputation(double reputation);
    map<FuzzySet, double> fuzzifyBlockValidity(double validity);
    map<FuzzySet, double> fuzzifyNetworkConsensus(double consensus);

    // Main BFT decision function (executes all 6 steps)
    double evaluateNodeTrust(double nodeReputation, double blockValidity, double networkConsensus);

    // Utility methods
    void printFuzzyRules();
    string fuzzySetToString(FuzzySet set);
};

#endif
