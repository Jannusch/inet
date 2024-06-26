//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_PASSIVEPACKETSOURCEBASE_H
#define __INET_PASSIVEPACKETSOURCEBASE_H

#include "inet/queueing/base/PacketSourceBase.h"
#include "inet/queueing/common/ActivePacketSinkRef.h"
#include "inet/queueing/contract/IActivePacketSink.h"
#include "inet/queueing/contract/IPassivePacketSource.h"

namespace inet {
namespace queueing {

class INET_API PassivePacketSourceBase : public PacketSourceBase, public virtual IPassivePacketSource
{
  protected:
    cGate *outputGate = nullptr;
    ActivePacketSinkRef collector;

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual bool supportsPacketPushing(const cGate *gate) const override { return false; }
    virtual bool supportsPacketPulling(const cGate *gate) const override { return outputGate == gate; }

    virtual bool canPullSomePacket(const cGate *gate) const override { return true; }

    virtual Packet *pullPacketStart(const cGate *gate, bps datarate) override { throw cRuntimeError("Invalid operation"); }
    virtual Packet *pullPacketEnd(const cGate *gate) override { throw cRuntimeError("Invalid operation"); }
    virtual Packet *pullPacketProgress(const cGate *gate, bps datarate, b position, b extraProcessableLength = b(0)) override { throw cRuntimeError("Invalid operation"); }
};

} // namespace queueing
} // namespace inet

#endif

