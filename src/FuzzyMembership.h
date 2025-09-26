#ifndef FUZZYMEMBERSHIP_H
#define FUZZYMEMBERSHIP_H

class FuzzyMembership {
public:
    // Basic membership function shapes
    static double triangularMembership(double x, double a, double b, double c);
    static double trapezoidalMembership(double x, double a, double b, double c, double d);
    static double gaussianMembership(double x, double center, double sigma);
    static double sigmoidMembership(double x, double center, double slope);

    // Specialized membership functions for BFT
    static double reputationLowMembership(double reputation);
    static double reputationMediumMembership(double reputation);
    static double reputationHighMembership(double reputation);

    static double validityLowMembership(double validity);
    static double validityMediumMembership(double validity);
    static double validityHighMembership(double validity);

    static double consensusLowMembership(double consensus);
    static double consensusMediumMembership(double consensus);
    static double consensusHighMembership(double consensus);
};

#endif
