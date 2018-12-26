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
        SetpointGeneric = 7,
        SetpointHighLevel = 8,
        Platform = 13,
        LinkLayer = 15
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
 