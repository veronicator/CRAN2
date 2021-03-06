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

#ifndef __C_RAN2_RRH_H_
#define __C_RAN2_RRH_H_

#include <omnetpp.h>
#include "PktMessage_m.h"
#include <queue>

using namespace omnetpp;

class RRH : public cSimpleModule
{
    cMessage* timer_ = nullptr;
    std::queue <PktMessage*> queue;     // RRH queue: needed when the "decompression unit" is active, to queue arriving packets
    bool dec_unit_in_use = false;       // indicate if the decompression is active or not
    PktMessage* pkt_in_dec = nullptr;    // pointer to the packet in the "decompression unit"

    // Signals
    simsignal_t packet_in_rrh_;
    simsignal_t packet_in_queue_;
    simsignal_t queueing_time_;
    simsignal_t response_time_;

    void forwardPkt();
    void decompressPkt();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
