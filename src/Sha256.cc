#include "SHA256.h"
#include <sstream>
#include <iomanip>

const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint32_t SHA256::H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

uint32_t SHA256::rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t SHA256::ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

uint32_t SHA256::maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t SHA256::sig0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t SHA256::sig1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint32_t SHA256::gamma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t SHA256::gamma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

std::vector<uint8_t> SHA256::hashBytes(const std::string& input) {
    // Convert string to bytes
    std::vector<uint8_t> data(input.begin(), input.end());
    
    // Pre-processing: adding padding bits
    size_t originalLength = data.size();
    data.push_back(0x80); // append bit '1' to message
    
    // append 0 <= k < 512 bits '0', such that the resulting message length in bits
    // is congruent to −64 ≡ 448 (mod 512)
    while ((data.size() % 64) != 56) {
        data.push_back(0x00);
    }
    
    // append original length in bits as 64-bit big-endian integer
    uint64_t bitLength = originalLength * 8;
    for (int i = 7; i >= 0; i--) {
        data.push_back((bitLength >> (i * 8)) & 0xFF);
    }
    
    // Initialize hash values
    uint32_t h[8];
    for (int i = 0; i < 8; i++) {
        h[i] = H0[i];
    }
    
    // Process the message in successive 512-bit chunks
    for (size_t chunk = 0; chunk < data.size(); chunk += 64) {
        uint32_t w[64];
        
        // Break chunk into sixteen 32-bit big-endian words
        for (int i = 0; i < 16; i++) {
            w[i] = (data[chunk + i * 4] << 24) |
                   (data[chunk + i * 4 + 1] << 16) |
                   (data[chunk + i * 4 + 2] << 8) |
                   (data[chunk + i * 4 + 3]);
        }
        
        // Extend the first 16 words into the remaining 48 words
        for (int i = 16; i < 64; i++) {
            w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
        }
        
        // Initialize working variables
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h7 = h[7];
        
        // Compression function main loop
        for (int i = 0; i < 64; i++) {
            uint32_t t1 = h7 + sig1(e) + ch(e, f, g) + K[i] + w[i];
            uint32_t t2 = sig0(a) + maj(a, b, c);
            h7 = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }
        
        // Add the compressed chunk to the current hash value
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += h7;
    }
    
    // Produce the final hash value as a 256-bit number
    std::vector<uint8_t> result;
    for (int i = 0; i < 8; i++) {
        result.push_back((h[i] >> 24) & 0xFF);
        result.push_back((h[i] >> 16) & 0xFF);
        result.push_back((h[i] >> 8) & 0xFF);
        result.push_back(h[i] & 0xFF);
    }
    
    return result;
}

std::string SHA256::hash(const std::string& input) {
    std::vector<uint8_t> hashBytes = SHA256::hashBytes(input);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : hashBytes) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    
    return ss.str();
}