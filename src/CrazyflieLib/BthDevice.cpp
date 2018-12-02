#include "pch.h"

#include "BthDevice.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

namespace winrt::bitcraze::crazyflielib::implementation
{
    constexpr winrt::guid crazyflieServiceGuid =
        winrt::guid(
            0x00000201,0x1c7f,0x4f9e,{0x94,0x7b,0x43,0xb7,0xc0,0x0a,0x9a,0x08});

    constexpr winrt::guid crtpCharacteristicGuid =
        winrt::guid(
            0x00000202,0x1c7f,0x4f9e,{0x94,0x7b,0x43,0xb7,0xc0,0x0a,0x9a,0x08});

    constexpr winrt::guid crtpUpCharacteristicGuid =
        winrt::guid(
            0x00000203,0x1c7f,0x4f9e,{0x94,0x7b,0x43,0xb7,0xc0,0x0a,0x9a,0x08});

    constexpr winrt::guid crtpDownCharacteristicGuid =
        winrt::guid(
            0x00000204,0x1c7f,0x4f9e,{0x94,0x7b,0x43,0xb7,0xc0,0x0a,0x9a,0x08});

    IAsyncOperation<IVector<winrt::hstring>>
        BthDevice::ScanInterfacesAsync()
    {
        // Scan for paired Crazyflie devices. Use the Crazyflie Service GUID
        // as the primary selector, but first query for and return the device
        // ID string, not the service ID string.

        std::vector<winrt::hstring> requestedProperties;
        requestedProperties.push_back(L"System.Devices.AepService.AepId");

        Windows::Foundation::Collections::IVector<winrt::hstring> props =
            winrt::single_threaded_vector<winrt::hstring>(
                std::move(requestedProperties));

        DeviceInformationCollection services
        {
            co_await DeviceInformation::FindAllAsync(
                GattDeviceService::GetDeviceSelectorFromUuid(
                    crazyflieServiceGuid), props)
        };

        std::vector<winrt::hstring> deviceIds;
        for (auto const& service : services)
        {
            winrt::hstring aepId =
                winrt::unbox_value<winrt::hstring>(
                    service.Properties().Lookup(
                        L"System.Devices.AepService.AepId"));

            DeviceInformation device
            {
                co_await DeviceInformation::CreateFromIdAsync(aepId)
            };

            deviceIds.push_back(device.Id());
        }
        return winrt::single_threaded_observable_vector<winrt::hstring>(
            std::move(deviceIds));
    }

    BthDevice::BthDevice(winrt::hstring device_name)
        : device_name_(device_name)
    { }

    IAsyncAction BthDevice::ConnectAsync()
    {
        this->device_ =
            std::make_shared<BluetoothLEDevice>(
                co_await BluetoothLEDevice::FromIdAsync(device_name_));

        // Allow use of cached mode for querying the services/attributes.
        // In the event the device is not physically present, this call
        // (ConnectAsync) may still complete successfully, but subsequent
        // reads/writes to characteristics will fail.
        auto getServResults =
            co_await this->device_->GetGattServicesAsync();

        if (getServResults.Status() != GattCommunicationStatus::Success)
        {
            throw winrt::hresult_error(
                HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE),
                L"Connection to Crazyflie failed. Is it powered on?");
        }

        for (auto const& s : getServResults.Services())
        {
            if (s.Uuid() == crazyflieServiceGuid)
            {
                this->crazyflieService_ =
                    std::make_shared<GattDeviceService>(s);
            }
        }

        if (!this->crazyflieService_)
        {
            throw winrt::hresult_error(
                E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND,
                L"Failed to get handle to the Crazyflie service"
            );
        }

        // Again allowing reading from the cache.
        auto getCharResults =
            co_await this->crazyflieService_->GetCharacteristicsAsync();

        if (getCharResults.Status() != GattCommunicationStatus::Success)
        {
            throw winrt::hresult_error(
                HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_AVAILABLE),
                L"Connection to Crazyflie failed. Is it powered on?");
        }

        for (auto const& c : getCharResults.Characteristics())
        {
            if (c.Uuid() == crtpCharacteristicGuid)
            {
                this->crtpCharacteristic_ =
                    std::make_shared<GattCharacteristic>(c);
            }
            else if (c.Uuid() == crtpUpCharacteristicGuid)
            {
                this->crtpUpCharacteristic_ =
                    std::make_shared<GattCharacteristic>(c);
            }
            else if (c.Uuid() == crtpDownCharacteristicGuid)
            {
                this->crtpDownCharacteristic_ =
                    std::make_shared<GattCharacteristic>(c);
            }
        }

        // Verify that all three characteristics were found
        if (!this->crtpCharacteristic_)
        {
            throw winrt::hresult_error(
                E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND,
                L"Failed to get handle to the CRTP characteristic"
            );
        }

        if (!this->crtpUpCharacteristic_)
        {
            throw winrt::hresult_error(
                E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND,
                L"Failed to get handle to the CRTPUP characteristic"
            );
        }

        if (!this->crtpDownCharacteristic_)
        {
            throw winrt::hresult_error(
                E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND,
                L"Failed to get handle to the CRTPDOWN characteristic"
            );
        }
    }
}