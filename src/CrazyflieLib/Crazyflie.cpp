#include "pch.h"
#include "Crazyflie.h"
#include "BthDevice.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

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
}
