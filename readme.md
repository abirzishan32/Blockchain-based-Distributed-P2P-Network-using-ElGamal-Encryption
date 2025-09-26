# Blockchain-based Distributed P2P Network with ElGamal Encryption

[![OMNeT++](https://img.shields.io/badge/OMNeT%2B%2B-6.0%2B-blue.svg)](https://omnetpp.org/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-green.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-LGPL--3.0-yellow.svg)](https://www.gnu.org/licenses/lgpl-3.0)

## Overview

This project implements a **decentralized peer-to-peer blockchain network** using OMNeT++ simulation framework. The network features advanced cryptographic security through **ElGamal public-key encryption** and intelligent Byzantine fault tolerance using **Fuzzy Logic Systems with Mamdani inference**.

### Key Features

- 🔗 **Decentralized P2P Architecture**: Multiple computers maintain synchronized blockchain copies without central authority
- 🔐 **ElGamal Encryption**: Advanced public-key cryptography replacing traditional SHA256 for superior block security
- 🧠 **Fuzzy Byzantine Fault Tolerance**: Intelligent consensus mechanism using Mamdani inference to handle malicious nodes
- 📡 **Encrypted Block Broadcasting**: Seamless encrypted block sharing across interconnected network nodes
- 🛡️ **Byzantine Node Simulation**: Multiple attack patterns including silent, corrupt, double-spending, and random malicious behaviors

## Architecture

### Network Topology
- **Fully Connected P2P Network**: Each node connects to every other node
- **Configurable Node Count**: Default 20 nodes (15 honest + 5 Byzantine)
- **Ring Layout Visualization**: Nodes arranged in a circular pattern for clear network observation

### Core Components

#### 1. Computer Node (`Computer.cc/.h`)
- **Node Types**: Honest, Byzantine Silent, Byzantine Corrupt, Byzantine Double-spending, Byzantine Random
- **Mining Capability**: Configurable mining intervals with exponential distribution
- **Network Communication**: Multi-port gates for P2P connectivity

#### 2. Blockchain System (`Blockchain.cc/.h`)
- **Block Structure**: Timestamp, data, previous hash, and ElGamal encryption
- **Chain Validation**: Cryptographic integrity verification
- **Consensus Management**: Distributed ledger synchronization

#### 3. ElGamal Cryptographic Engine (`ElGamal.cc/.h`)
- **Key Generation**: Prime-based public-private key pairs
- **Block Encryption**: Character-by-character encryption with random padding
- **Secure Communication**: End-to-end encrypted block transmission

#### 4. Fuzzy BFT Consensus (`FuzzyBFT.cc/.h`)
- **Input Parameters**: Node reputation, block validity, network consensus
- **Fuzzy Sets**: Low, Medium, High membership functions
- **Mamdani Inference**: Rule-based decision making for trust evaluation
- **Output**: Trust level for consensus participation

#### 5. Byzantine Node Behaviors (`ByzantineNode.cc/.h`)
- **Silent Attack**: Nodes that receive but don't participate
- **Corruption Attack**: Nodes that modify block data
- **Double-spending**: Nodes attempting duplicate transactions
- **Random Behavior**: Unpredictable malicious actions

## Installation & Setup

### Prerequisites
- **OMNeT++ 6.0+** - [Download](https://omnetpp.org/)
- **C++17 Compatible Compiler** (GCC 7+, Clang 5+, or MSVC 2017+)
- **Make** build system

### Building the Project

1. **Clone the repository:**
   ```bash
   git clone https://github.com/abirzishan32/Blockchain-P2P-Network.git
   cd "Network Project"
   ```

2. **Generate Makefiles:**
   ```bash
   make makefiles
   ```

3. **Compile the project:**
   ```bash
   make
   ```

4. **Clean build (if needed):**
   ```bash
   make clean
   # or complete cleanup
   make cleanall
   ```

## Running Simulations

### Command Line Execution
```bash
cd simulations
../src/BlockchainProject -u Cmdenv -c General
```

### GUI Execution
```bash
cd simulations
../src/BlockchainProject
```

### Configuration Parameters

Edit `simulations/omnetpp.ini` to customize:

```ini
# Network size
*.numNodes = 20

# Byzantine node distribution
*.computer[0..14].nodeType = 0    # Honest nodes
*.computer[15..16].nodeType = 1   # Byzantine silent
*.computer[17..18].nodeType = 2   # Byzantine corrupt
*.computer[19].nodeType = 3       # Byzantine double-spending

# Mining intervals
*.computer[*].miningInterval = exponential(20s)
```

## Technical Implementation

### ElGamal Encryption Process

1. **Key Generation**: Generate prime `p`, generator `e1`, private key `d`, public key `e2 = e1^d mod p`
2. **Block Encryption**: Each character encrypted as `(c1, c2) = (e1^r mod p, m*e2^r mod p)`
3. **Decryption**: Recover message `m = c2 * (c1^d)^(-1) mod p`

### Fuzzy BFT Consensus Algorithm

1. **Fuzzification**: Convert crisp inputs (reputation, validity, consensus) to fuzzy sets
2. **Rule Evaluation**: Apply Mamdani inference rules for trust calculation
3. **Defuzzification**: Convert fuzzy output to crisp trust level
4. **Consensus Decision**: Accept/reject blocks based on aggregated trust scores

### Network Communication Flow

```
Node A creates block → ElGamal encryption → Broadcast to all peers → 
Fuzzy BFT validation → Consensus decision → Blockchain update
```

## File Structure

```
BlockchainProject/
├── src/                          # Source code
│   ├── Computer.{cc,h,ned}       # Network node implementation
│   ├── Blockchain.{cc,h}         # Blockchain core logic
│   ├── Block.{cc,h}              # Block data structure
│   ├── ElGamal.{cc,h}            # Cryptographic engine
│   ├── FuzzyBFT.{cc,h}           # Consensus algorithm
│   ├── FuzzyMembership.{cc,h}    # Fuzzy logic functions
│   ├── ByzantineNode.{cc,h}      # Attack simulation
│   ├── PrimeGenerator.{cc,h}     # Cryptographic utilities
│   ├── Network.ned               # Network topology
│   └── package.ned               # Package definition
├── simulations/                  # Simulation configurations
│   ├── omnetpp.ini              # Main configuration
│   ├── package.ned              # Simulation package
│   └── results/                 # Output data
├── Makefile                     # Build configuration
└── README.md                    # This file
```

## Research Applications

This simulation framework is ideal for:

- **Blockchain Security Research**: Testing cryptographic alternatives to SHA256
- **Byzantine Fault Tolerance Studies**: Analyzing fuzzy logic approaches to consensus
- **P2P Network Analysis**: Understanding decentralized network behavior
- **Cryptographic Protocol Evaluation**: Comparing ElGamal vs traditional methods
- **Consensus Algorithm Development**: Prototyping new BFT mechanisms




## Contact

**Abir Zishan** - [@abirzishan32](https://github.com/abirzishan32)

Project Link: [https://github.com/abirzishan32/Blockchain-P2P-Network](https://github.com/abirzishan32/Blockchain-P2P-Network)