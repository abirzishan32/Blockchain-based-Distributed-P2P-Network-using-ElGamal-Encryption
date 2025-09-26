#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <vector>
#include <cstdint>

class SHA256 {
private:
    static const uint32_t K[64];
    static const uint32_t H0[8];
    
    static uint32_t rotr(uint32_t x, int n);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t sig0(uint32_t x);
    static uint32_t sig1(uint32_t x);
    static uint32_t gamma0(uint32_t x);
    static uint32_t gamma1(uint32_t x);

public:
    static std::string hash(const std::string& input);
    static std::vector<uint8_t> hashBytes(const std::string& input);
};

#endif