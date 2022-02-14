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

#include "BBU.h"

Define_Module(BBU);

void BBU::initialize()
{
    tx_channel = gate("out")->getTransmissionChannel();
    msg_timer = new cMessage();
    gate_size = gateSize("out");

    occupation_queue_ = registerSignal("occupationQueue");
    queueing_time_ = registerSignal("queueingTime");
    response_time_ = registerSignal("responseTime");

}

void BBU::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage() && !pkt_queue->isEmpty()) {
        // extract packet from queue and send
        if(!tx_channel->isBusy()) {
            cPacket *pkt = pkt_queue->pop();
            sendPacket(pkt);
        } else {
            scheduleAt(tx_channel->getTransmissionFinishTime(), msg_timer);
        }
    } else {
        // new packet from AS
        PktMessage *pkt = check_and_cast<PktMessage*>(msg);
        pkt->setTimestamp();
        simtime_t enqueue_time = simTime();
        pkt->setEnqueue_time(enqueue_time);

        // the packet is queued only if the transmitted channel is busy or there are other packets in the queue
        if(tx_channel->isBusy() || !pkt_queue->isEmpty()) {
            pkt_queue->insert(pkt);
        } else {
            // idle channel and empty queue
            sendPacket(pkt);
        }
        emit(occupation_queue_, (long)(pkt_queue->getByteLength()));
    }
}

void BBU::finish()
{
    pkt_queue->clear();
    cancelAndDelete(msg_timer);
}

int BBU::compressPacket(cPacket *pkt) {
    int new_size = ceil(pkt->getByteLength() * ((100 - par("compression_rate").intValue()) / 100));
    pkt->setByteLength(new_size);
    return new_size;
}


void BBU::sendPacket(cMessage *msg) {
    PktMessage *pkt = check_and_cast<PktMessage*>(msg);

    simtime_t queueing_t = simTime() - pkt->getTimestamp();
    emit(queueing_time_, queueing_t);
    simtime_t response_t = queueing_t + tx_channel->calculateDuration(pkt);
    //emit(response_time_, response_t);

    int index_gate = pkt->getTarget_cell();
    EV << index_gate << " index" << endl;
    if(index_gate < 0 || index_gate >= gate_size) {
        error("BBU: no corresponding target gate");
    }

    if(par("compression_used").boolValue()) {
        int new_size = compressPacket(pkt);
        EV << "size compressed: " << new_size << endl;
    }

    send(pkt, "out", index_gate);
    scheduleAt(tx_channel->getTransmissionFinishTime(), msg_timer);

    emit(response_time_, response_t);
}






