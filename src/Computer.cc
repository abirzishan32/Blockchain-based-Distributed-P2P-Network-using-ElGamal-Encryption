#include <omnetpp.h>
#include "Blockchain.h"
#include <sstream>

using namespace omnetpp;

class Computer : public cSimpleModule {
private:
    Blockchain blockchain;
    int nodeId;
    cMessage *blockTimer;

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void createNewBlock();
    void broadcastNewBlock(const std::string& blockData);
    void syncBlockchain();
    void requestBlockchain(int fromGate);
    void sendBlockchain(int toGate);
};

Define_Module(Computer);

void Computer::initialize() {
    nodeId = par("nodeId");
    blockTimer = new cMessage("blockTimer");

    // Schedule first block creation attempt
    scheduleAt(simTime() + par("miningInterval"), blockTimer);

    EV << "Computer " << nodeId << " initialized with blockchain\n";
}

void Computer::handleMessage(cMessage *msg) {
    if (msg == blockTimer) {
        createNewBlock();
        scheduleAt(simTime() + par("miningInterval"), blockTimer);
    }
    else if (strcmp(msg->getName(), "newBlock") == 0) {
        // Received a new block from peer
        if (msg->hasPar("blockData")) {
            std::string blockData = msg->par("blockData").stringValue();

            // Simple validation - in real implementation, validate the block properly
            EV << "Computer " << nodeId << " received new block data\n";

            // For simplicity, just acknowledge receipt
            // In real implementation, validate and add block if valid
        }
        delete msg;
    }
    else if (strcmp(msg->getName(), "syncRequest") == 0) {
        // Send our blockchain to requesting peer
        int fromGate = msg->getArrivalGate()->getIndex();
        sendBlockchain(fromGate);
        delete msg;
    }
    else if (strcmp(msg->getName(), "syncResponse") == 0) {
        // Received blockchain from peer
        if (msg->hasPar("chainData")) {
            std::string chainData = msg->par("chainData").stringValue();

            // Simple sync logic - in real implementation, validate and replace if longer
            EV << "Computer " << nodeId << " received blockchain sync data\n";
        }
        delete msg;
    }
    else {
        delete msg;
    }
}

void Computer::createNewBlock() {
    std::stringstream ss;
    ss << "Block data from node " << nodeId << " at time " << simTime();

    blockchain.addBlock(ss.str());

    EV << "Computer " << nodeId << " created new block " << (blockchain.getChainLength() - 1) << "\n";

    // Broadcast new block to all peers
    Block* newBlock = blockchain.getLatestBlock();
    if (newBlock) {
        broadcastNewBlock(newBlock->serialize());
    }

    // Periodically sync with peers
    if (blockchain.getChainLength() % 3 == 0) {
        syncBlockchain();
    }
}

void Computer::broadcastNewBlock(const std::string& blockData) {
    for (int i = 0; i < gateSize("port"); i++) {
        if (gate("port$o", i)->isConnected()) {
            cMessage *msg = new cMessage("newBlock");
            msg->addPar("blockData") = blockData.c_str();
            send(msg, "port$o", i);
        }
    }
}

void Computer::syncBlockchain() {
    for (int i = 0; i < gateSize("port"); i++) {
        if (gate("port$o", i)->isConnected()) {
            requestBlockchain(i);
        }
    }
}

void Computer::requestBlockchain(int fromGate) {
    cMessage *msg = new cMessage("syncRequest");
    send(msg, "port$o", fromGate);
}

void Computer::sendBlockchain(int toGate) {
    if (gate("port$o", toGate)->isConnected()) {
        cMessage *msg = new cMessage("syncResponse");
        msg->addPar("chainData") = blockchain.serialize().c_str();
        send(msg, "port$o", toGate);
    }
}

void Computer::finish() {
    cancelAndDelete(blockTimer);
    EV << "Computer " << nodeId << " final blockchain length: " << blockchain.getChainLength() << "\n";
}
