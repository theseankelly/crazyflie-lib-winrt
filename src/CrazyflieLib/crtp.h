#pragma once

#include <cstdint>

namespace winrt::bitcraze::crazyflielib::implementation
{
    enum class CrtpPort : std::uint8_t
    {
        Console = 0,
        Parameters = 2,
        Commander = 3,
        Memory = 4,
        Log = 5,
        Localization = 6,
        GenericSetpoint = 7,
        HighLevelSetpoint= 8,
        Platform = 13,
        LinkLayer = 15
    };

    enum class CrtpGenericSetpointId : std::uint8_t
    {
        Stop = 0,
        VelocityWorld = 1,
        ZDistance = 2,
        CPPMEmulation = 3,
        AltitudeHold = 4,
        Hover = 5,
        FullState = 6,
        Position = 7
    };

    union CrtpHeader
    {
        struct
        {
            std::uint8_t channel : 2;
            std::uint8_t link : 2;
            std::uint8_t port : 4;
        } fields;
        std::uint8_t as_byte;
    };

    static_assert(sizeof(CrtpHeader) == 1);
}
