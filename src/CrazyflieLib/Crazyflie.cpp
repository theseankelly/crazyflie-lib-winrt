#include "pch.h"
#include "Crazyflie.h"
#include "BthDevice.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage::Streams;

namespace winrt::bitcraze::crazyflielib::implementation
{
    IAsyncOperation<IVector<winrt::hstring>>
    Crazyflie::ScanInterfacesAsync()
    {
        return BthDevice::ScanInterfacesAsync();
    }

    Crazyflie::Crazyflie(winrt::hstring device_name)
        : device_name_(device_name)
    { }

    IAsyncAction
    Crazyflie::ConnectAsync()
    {
        this->bluetooth_device_ =
            std::make_shared<BthDevice>(this->device_name_);

        return this->bluetooth_device_->ConnectAsync();
    }

    IAsyncOperation<bool>
    Crazyflie::SendCommanderPacketAsync(
        float roll,
        float pitch,
        float yaw,
        float thrust)
    {
        DataWriter payload;
        payload.WriteSingle(roll);
        payload.WriteSingle(pitch);
        payload.WriteSingle(yaw);
        payload.WriteUInt16(static_cast<std::uint16_t>(thrust * UINT16_MAX));

        return this->bluetooth_device_->SendAsync(
            CrtpPort::Commander, payload.DetachBuffer());
    }
}
