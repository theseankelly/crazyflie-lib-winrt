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

    union CrtpUpDownCtrl
    {
        struct
        {
            std::uint8_t length : 4;
            std::uint8_t pid : 3;
            std::uint8_t start : 1;
        } fields;
        std::uint8_t as_byte;
    };

    // Thinking about a 'crtp' class. It would satisfy the following features:
    // - The device interface's Send() function would take a CrtpPacket instead of IBuffer
    // - member data - port/header, data
    // - helper functions - serialize, deserialize
    //
    // How does sending data look (with bluetooth?)
    // - Create a CrtpPacket()
    // - set the header vals
    // - give it data (std::vector?)
    // - pass to SendAsync
    //      - SendAsync() transforms as necessary.

    class CrtpPacket
    {
    public:
        CrtpPacket();
        CrtpPacket(CrtpPort p);
        CrtpPacket(std::vector<byte>);

        // Getters/setters
        CrtpPort port();
        void port(CrtpPort p);
    private:
        CrtpHeader header_;

        std::vector<byte> data_;
    };


}
