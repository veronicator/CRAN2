//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "AS.h"

Define_Module(AS);

void AS::initialize()
{
    numTarget = par("N_target").intValue();

    sizeMean = par("sizeMean");
    if(sizeMean < 0){
        error("Error in  Size Mean Value Extraction: The value is negative");
    }

    sizeVariance = par("sizeVariance");
    if(timeMean < 0){
        error("Error in Size Variance Value Extraction: The value is negative");
    }

    timeMean = par("timeMean");
    if(timeMean < 0){
        error("Error in Time Mean Value Extraction: The value is negative");
    }

    // Create a msg
    generate = new cMessage();
    generate_delay();
}

//Function that is activated after the waiting time. It needs to create and prepare a packet to send on the "out" link to the BBU
//After the send is done will call generate_delay to have another wait time
void AS::handleMessage(cMessage *msg)
{
    int size;

    // Generating a "random" size for the packet
    if(par("sizeDistribution").intValue() == 1) {  // exponential
        size = (int)exponential(sizeMean, SIZE_RNG);
    } else {  // lognormal
        size = (int)lognormal(sizeMean, sizeVariance, SIZE_RNG);
    }

    // Create a new packet with size and the cell to reach in the interval [0, N-1]
    PktMessage* pkt = new PktMessage();
    pkt->setByteLength(size);
    pkt->setTarget_cell(intuniform(0, numTarget-1, TARGET_RNG));

    // Send the generated pkt on the "out" link, also the only one available
    send(pkt, "out");
    //I proceed to wait another pkt generation cycle
    generate_delay();
}

//Function that generate an amount of time to wait between the creation of two packets to send to the BBU
void AS::generate_delay() {
    simtime_t time;

    if(par("timeDistribution").intValue() == 1) {  // exponential
        //Generating a "random" amount of time to wait
        time = (simtime_t)exponential(timeMean, TIME_RNG);
    } else {    // constant
        time = timeMean;
    }

    // Send to myself a msg to notify that i have to send a packet to the BBU
    scheduleAt(simTime() + time, generate);
    EV << "generated time: " << time << endl;
}

//Function to stop the packet generation
void AS::finish() {
    cancelAndDelete(generate);
}
