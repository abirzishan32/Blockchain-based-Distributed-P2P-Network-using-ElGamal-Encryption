#include "FuzzyMembership.h"
#include <cmath>
#include <algorithm>

// Basic triangular membership function
double FuzzyMembership::triangularMembership(double x, double a, double b, double c) {
    if (x <= a || x >= c) return 0.0;
    if (x == b) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (c - x) / (c - b);
}

// Basic trapezoidal membership function
double FuzzyMembership::trapezoidalMembership(double x, double a, double b, double c, double d) {
    if (x <= a || x >= d) return 0.0;
    if (x >= b && x <= c) return 1.0;
    if (x < b) return (x - a) / (b - a);
    return (d - x) / (d - c);
}

// Gaussian membership function
double FuzzyMembership::gaussianMembership(double x, double center, double sigma) {
    return std::exp(-0.5 * std::pow((x - center) / sigma, 2));
}

// Sigmoid membership function
double FuzzyMembership::sigmoidMembership(double x, double center, double slope) {
    return 1.0 / (1.0 + std::exp(-slope * (x - center)));
}

// Specialized membership functions for Byzantine Fault Tolerance

// Node Reputation membership functions
double FuzzyMembership::reputationLowMembership(double reputation) {
    // Low reputation: 0.0 to 0.4 with peak at 0.2
    return trapezoidalMembership(reputation, 0.0, 0.0, 0.2, 0.4);
}

double FuzzyMembership::reputationMediumMembership(double reputation) {
    // Medium reputation: 0.2 to 0.8 with peak at 0.5
    return triangularMembership(reputation, 0.2, 0.5, 0.8);
}

double FuzzyMembership::reputationHighMembership(double reputation) {
    // High reputation: 0.6 to 1.0 with peak at 0.9
    return trapezoidalMembership(reputation, 0.6, 0.8, 1.0, 1.0);
}

// Block Validity membership functions
double FuzzyMembership::validityLowMembership(double validity) {
    // Low validity: 0.0 to 0.5 (clearly invalid blocks)
    return trapezoidalMembership(validity, 0.0, 0.0, 0.3, 0.5);
}

double FuzzyMembership::validityMediumMembership(double validity) {
    // Medium validity: 0.3 to 0.8 (questionable blocks)
    return triangularMembership(validity, 0.3, 0.6, 0.8);
}

double FuzzyMembership::validityHighMembership(double validity) {
    // High validity: 0.7 to 1.0 (clearly valid blocks)
    return trapezoidalMembership(validity, 0.7, 0.85, 1.0, 1.0);
}

// Network Consensus membership functions (critical for BFT)
double FuzzyMembership::consensusLowMembership(double consensus) {
    // Low consensus: 0% to 40% (Byzantine attack scenario)
    return trapezoidalMembership(consensus, 0.0, 0.0, 0.25, 0.4);
}

double FuzzyMembership::consensusMediumMembership(double consensus) {
    // Medium consensus: 30% to 70% (uncertain state)
    return triangularMembership(consensus, 0.3, 0.5, 0.7);
}

double FuzzyMembership::consensusHighMembership(double consensus) {
    // High consensus: 60% to 100% (BFT safety threshold)
    return trapezoidalMembership(consensus, 0.6, 0.75, 1.0, 1.0);
}
