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
         * Asynchronously scans all supported interfaces for
         * available Crazyflie devices.
         *
         * @return Vector of device ID strings.
         */
        static
        IAsyncOperation<IVector<winrt::hstring>>
        ScanInterfacesAsync();

    private:
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
