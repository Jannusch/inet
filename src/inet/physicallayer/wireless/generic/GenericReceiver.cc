//
// Copyright (C) 2013 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#include "inet/physicallayer/wireless/generic/GenericReceiver.h"

#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"
#include "inet/physicallayer/wireless/common/radio/packetlevel/ListeningDecision.h"
#include "inet/physicallayer/wireless/common/radio/packetlevel/ReceptionDecision.h"
#include "inet/physicallayer/wireless/common/analogmodel/packetlevel/UnitDiskNoise.h"
#include "inet/physicallayer/wireless/common/analogmodel/packetlevel/UnitDiskReceptionAnalogModel.h"

namespace inet {
namespace physicallayer {

Define_Module(GenericReceiver);

GenericReceiver::GenericReceiver() :
    ReceiverBase(),
    ignoreInterference(false)
{
}

void GenericReceiver::initialize(int stage)
{
    ReceiverBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
        ignoreInterference = par("ignoreInterference");
}

std::ostream& GenericReceiver::printToStream(std::ostream& stream, int level, int evFlags) const
{
    stream << "GenericReceiver";
    if (level <= PRINT_LEVEL_INFO)
        stream << (ignoreInterference ? ", ignoring interference" : ", considering interference");
    return stream;
}

bool GenericReceiver::computeIsReceptionPossible(const IListening *listening, const IReception *reception, IRadioSignal::SignalPart part) const
{
    return getAnalogModel()->computeIsReceptionPossible(listening, reception, part);
}

bool GenericReceiver::computeIsReceptionSuccessful(const IListening *listening, const IReception *reception, IRadioSignal::SignalPart part, const IInterference *interference, const ISnir *snir) const
{
    auto power = check_and_cast<const UnitDiskReceptionAnalogModel *>(reception->getNewAnalogModel())->getPower();
    if (power == UnitDiskReceptionAnalogModel::POWER_RECEIVABLE) {
        if (ignoreInterference)
            return true;
        else {
            auto startTime = reception->getStartTime(part);
            auto endTime = reception->getEndTime(part);
            auto interferingReceptions = interference->getInterferingReceptions();
            for (auto interferingReception : *interferingReceptions) {
                auto interferingPower = check_and_cast<const UnitDiskReceptionAnalogModel *>(interferingReception->getNewAnalogModel())->getPower();
                if (interferingPower >= UnitDiskReceptionAnalogModel::POWER_INTERFERING && startTime <= interferingReception->getEndTime() && endTime >= interferingReception->getStartTime())
                    return false;
            }
            return true;
        }
    }
    else
        return false;
}

const IListening *GenericReceiver::createListening(const IRadio *radio, const simtime_t startTime, const simtime_t endTime, const Coord& startPosition, const Coord& endPosition) const
{
    return getAnalogModel()->createListening(radio, startTime, endTime, startPosition, endPosition);
}

const IListeningDecision *GenericReceiver::computeListeningDecision(const IListening *listening, const IInterference *interference) const
{
    return getAnalogModel()->computeListeningDecision(listening, interference);
}

//const IReceptionResult *GenericReceiver::computeReceptionResult(const IListening *listening, const IReception *reception, const IInterference *interference, const ISnir *snir, const std::vector<const IReceptionDecision *> *decisions) const
//{
//    auto noise = check_and_cast_nullable<const UnitDiskNoise *>(snir->getNoise());
//    double errorRate = check_and_cast<const UnitDiskReceptionAnalogModel *>(reception->getNewAnalogModel())->getPower() == UnitDiskReceptionAnalogModel::POWER_RECEIVABLE && (noise == nullptr || !noise->isInterfering()) ? 0 : 1;
//    auto receptionResult = ReceiverBase::computeReceptionResult(listening, reception, interference, snir, decisions);
//    auto errorRateInd = const_cast<Packet *>(receptionResult->getPacket())->addTagIfAbsent<ErrorRateInd>();
//    errorRateInd->setSymbolErrorRate(errorRate);
//    errorRateInd->setBitErrorRate(errorRate);
//    errorRateInd->setPacketErrorRate(errorRate);
//    return receptionResult;
//}

} // namespace physicallayer
} // namespace inet
