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

#ifndef __C_RAN2_CELL_H_
#define __C_RAN2_CELL_H_

#include <omnetpp.h>
#include "PktMessage_m.h"

using namespace omnetpp;

/*
 * module to collect the final statistics
 */
class Cell : public cSimpleModule
{
    simtime_t arrivalTime_;
    simsignal_t endToEndDelay_;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
