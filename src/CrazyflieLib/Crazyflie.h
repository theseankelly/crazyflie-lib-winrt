#pragma once

#include "pch.h"
#include "Crazyflie.g.h"

#include "BthDevice.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::bitcraze::crazyflielib::implementation
{
    /**
     * Class for managing interactions with a Crazyflie.
     */
    class Crazyflie : public CrazyflieT<Crazyflie>
    {
    public:
        /**
         * Constructs a Crazyflie instance.
         *
         * @param device_name Device ID string to associate with the instance.
         */
        Crazyflie(winrt::hstring device_name);

        /**
         * Asynchronously creates connection to this Crazyflie instance.
         */
        IAsyncAction
        ConnectAsync();

        /**
         * Asynchronously sends basic commander packet
         */
        IAsyncOperation<bool>
        SendCommanderPacketAsync(
            float roll,
            float pitch,
            float yaw,
            float thrust);

        /**
         * Asynchronously scans all supported interfaces for
         * available Crazyflie devices.
         *
         * @return Vector of device ID strings.
         */
        static
        IAsyncOperation<IVector<winrt::hstring>>
        ScanInterfacesAsync();

    private:
        static constexpr float MAX_ROLL = 30;
        static constexpr float MAX_PITCH = 30;
        static constexpr float MAX_YAW = 200;
        static constexpr std::uint16_t MAX_THRUST = UINT16_MAX;

        winrt::hstring device_name_;
        std::shared_ptr<BthDevice> bluetooth_device_;
    };
}

namespace winrt::bitcraze::crazyflielib::factory_implementation
{
    class Crazyflie : public CrazyflieT<Crazyflie, implementation::Crazyflie>
    {
    };
}
