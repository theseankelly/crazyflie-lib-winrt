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
         * Asynchronously initializes this Crazyflie instance.
         *
         * @return CrazyflieStatus return code
         */
        IAsyncOperation<CrazyflieStatus>
        InitializeAsync();

        /**
         * Asynchronously sends basic RPYT commander packet
         *
         * @param roll Signed percent (-1,1) to apply on the roll axis
         *
         * @param pitch Signed percent (-1,1) to apply on the pitch axis
         *
         * @param yaw Signed percent (-1,1) to apply on the yaw axis
         *
         * @param thrust Unsigned percent (0,1) to apply on the thrust axis
         */
        IAsyncOperation<CrazyflieStatus>
        SendCommanderPacketAsync(
            float roll,
            float pitch,
            float yaw,
            float thrust);

        /**
         * Asynchronously sends hover setpoint packet
         *
         * @param roll Signed percent (-1,1) to apply on the roll axis
         *
         * @param pitch Signed percent (-1,1) to apply on the pitch axis
         *
         * @param yaw Signed percent (-1,1) to apply on the yaw axis
         *
         * @param thrust Unsigned percent (0,1) to apply on the thrust axis
         */
        IAsyncOperation<CrazyflieStatus>
        SendCommanderHoverPacketAsync(
            float vx,
            float vy,
            float yawrate,
            float zdist);

        /**
         * Asynchronously sends stop setpoint packet
         */
        IAsyncOperation<CrazyflieStatus>
        SendCommanderStopPacketAsync();

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
