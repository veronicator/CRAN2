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

#include "RRH.h"
Define_Module(RRH);

void RRH::initialize()
{
    timer_ = new cMessage("timer");

    packet_in_rrh_ = registerSignal("packetInRRH");
    packet_in_queue_ = registerSignal("rrhPacketInQueue");
    queueing_time_ = registerSignal("rrhQueueingTime");
    response_time_ = registerSignal("rrhResponseTime");
}

void RRH::handleMessage(cMessage *msg)
{
    //Two cases: if a packet ended the decompression or has been sent by the BBU
    if(msg->isSelfMessage()) {
        //Decompression ended -> the "decompression unit" is free
        dec_unit_in_use = false;
        //The packet is forwarded to the cell
        forwardPkt();

        if(queue.size() > 0)
            decompressPkt();

    } else {
        // new packet from bbu
        // E[N]_rrh
        long num_pkt = static_cast<long>(queue.size() + (int)dec_unit_in_use);
        emit(packet_in_rrh_, num_pkt);

        //The new packet is pushed in queue
        PktMessage* new_pkt = check_and_cast<PktMessage*>(msg);
        new_pkt->setTimestamp();
        queue.push(new_pkt);

        //If the "decompression unit" is free and a packet is waiting for it
        if(queue.size() == 1 && !dec_unit_in_use) {
            //The first packet in queue begin the decompression
            decompressPkt();
        }
    }
}

//Free the memory
void RRH::finish() {
    cancelAndDelete(timer_);

    while(!queue.empty()) {
        PktMessage *pkt = queue.front();
        queue.pop();
        delete pkt;
    }
    delete pkt_in_dec;
}

//Function that forwards the packet to the cell connected to the RRH
void RRH::forwardPkt()
{
    //The message pointed by pkt_in_dec was decompressed and is ready to be sent
    PktMessage *to_transmit = pkt_in_dec;
    pkt_in_dec = nullptr;
    send(to_transmit, "out");
}

//Function that simulates the "decompression unit"
void RRH::decompressPkt()
{
    //The "decompression unit" is now in use
    dec_unit_in_use = true;
    //FIFO policy
    pkt_in_dec = queue.front();
    queue.pop();

    simtime_t queueing_t = simTime() - pkt_in_dec->getArrivalTime();
    emit(queueing_time_, queueing_t);
    long in_queue = static_cast<long>(queue.size());
    emit(packet_in_queue_, in_queue);

    simtime_t decompression_time = 0;

    //Check if the decompression is in use
    if(par("compression_used").boolValue()) {
        //Get the alfa parameter
        double alfa = par("alfa").doubleValue();
        //Calculate the decompression time
        double to_wait = (alfa * 50) * (par("compression_ratio").doubleValue());
        decompression_time = SimTime(to_wait, (SimTimeUnit)-3);
        //The size of the packet is set back to its original value
        int new_size = ceil(pkt_in_dec->getByteLength() / (1 - par("compression_ratio").doubleValue() / 100));
        pkt_in_dec->setByteLength(new_size);

    }
    scheduleAt(simTime() + decompression_time, timer_);
    emit(response_time_, queueing_t + decompression_time);
}
