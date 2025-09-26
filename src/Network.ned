package blockchainproject;

network BlockchainNetwork {
    parameters:
        int numNodes = default(20);  // Match with omnetpp.ini
        @display("bgb=800,600;bgi=background/terrain,s");
    submodules:
        computer[numNodes]: Computer {
            nodeId = index;
            @display("p=,,ring");  // Arrange computers in a ring layout
        }
    connections:
        // Create a fully connected network - each computer connects to every other computer
        for i=0..numNodes-2, for j=i+1..numNodes-1 {
            computer[i].port++ <--> computer[j].port++;
        }
}