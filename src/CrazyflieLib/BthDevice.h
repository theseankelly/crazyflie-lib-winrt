#pragma once
#include "pch.h"
#include "crtp.h"

#include "winrt/bitcraze.crazyflielib.h"

using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage::Streams;

namespace winrt::bitcraze::crazyflielib::implementation
{
    class BthDevice
    {
    public:
        /**
         * Class method to asynchronously search for paired Crazyflie devices.
         *
         * @return Async operation returns a vector of device ID strings for
         *         all found devices.
         */
        static IAsyncOperation<IVector<winrt::hstring>>
        ScanInterfacesAsync();

        /**
         * Constructor
         *
         * @param device_name Device ID string to use for establishing
         *        a Bluetooth device
         */
        BthDevice(winrt::hstring device_name);

        /**
         * Asynchronously initializes the Bluetooth connection, service handle
         * and characteristic handles.
         *
         * @note Connection management is handled by the operating system. The
         *       Bluetooth device is not necessarily in the 'connected' state
         *       following this call, but service/characteristic handles have
         *       been initialized for use.
         *
         * @note Due to OS level caching of services/characteristics, success
         *       from this routine does not guarantee device availability.
         *       Read/Write operations to characteristics must be monitered for
         *       success.
         */
        IAsyncOperation<CrazyflieStatus> InitializeAsync();

        IAsyncOperation<CrazyflieStatus> SendAsync(
            CrtpPort port_id,
            IBuffer data);

    protected:
        winrt::hstring device_name_;

        // CRTP characteristics
        std::shared_ptr<GattCharacteristic> crtpCharacteristic_ = nullptr;
        std::shared_ptr<GattCharacteristic> crtpUpCharacteristic_ = nullptr;
        std::shared_ptr<GattCharacteristic> crtpDownCharacteristic_ = nullptr;

    private:
        bool initialized_ = false;
    };
}

