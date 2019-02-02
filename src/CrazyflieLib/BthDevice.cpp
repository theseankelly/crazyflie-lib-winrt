#include "pch.h"

#include "BthDevice.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace winrt::Windows::Storage::Streams;

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

        // AepId is necessary to obtain the Device ID
        Windows::Foundation::Collections::IVector<winrt::hstring> props
        {
            winrt::single_threaded_vector<winrt::hstring>(
                {L"System.Devices.AepService.AepId"})
        };

        DeviceInformationCollection services
        {
            co_await DeviceInformation::FindAllAsync(
                GattDeviceService::GetDeviceSelectorFromUuid(crazyflieServiceGuid), props)
        };

        // Build a list of device IDs from the returned GATT services
        std::vector<winrt::hstring> deviceIds;
        for (auto const& service : services)
        {
            winrt::hstring aepId
            {
                winrt::unbox_value<winrt::hstring>(
                    service.Properties().Lookup(
                        L"System.Devices.AepService.AepId"))
            };

            DeviceInformation device
            {
                co_await DeviceInformation::CreateFromIdAsync(aepId)
            };

            deviceIds.push_back(device.Id());
        }

        return winrt::single_threaded_observable_vector<winrt::hstring>(std::move(deviceIds));
    }

    BthDevice::BthDevice(winrt::hstring device_name)
        : device_name_(device_name)
    { }

    IAsyncOperation<CrazyflieStatus> BthDevice::InitializeAsync()
    {
        if (this->initialized_)
        {
            co_return CrazyflieStatus::Success;
        }

        // BluetoothLEDevice::FromIdAsync throws E_INVALIDARG for invalid
        // device name strings.
        BluetoothLEDevice device = nullptr;
        try
        {
            device = co_await BluetoothLEDevice::FromIdAsync(device_name_);
        }
        catch (winrt::hresult_error const& ex)
        {
            HRESULT hr = ex.to_abi();
            if (hr == E_INVALIDARG)
            {
                return CrazyflieStatus::InvalidDeviceId;
            }
            else
            {
                return CrazyflieStatus::Failure;
            }
        }

        // CRTP Service
        GattDeviceServicesResult serviceResults =
            co_await device.GetGattServicesForUuidAsync(crazyflieServiceGuid);
        if ((serviceResults.Status() == GattCommunicationStatus::Success) &&
            (serviceResults.Services().Size() == 1))
        {
            // CRTP Characteristic
            auto crazyflieService = serviceResults.Services().GetAt(0);
            GattCharacteristicsResult characteristicResults =
                co_await crazyflieService.GetCharacteristicsForUuidAsync(crtpCharacteristicGuid);
            if ((characteristicResults.Status() == GattCommunicationStatus::Success) &&
                (characteristicResults.Characteristics().Size()  == 1))
            {
                this->crtpCharacteristic_ =
                    std::make_shared<GattCharacteristic>(
                        characteristicResults.Characteristics().GetAt(0));
            }
            else
            {
                co_return CrazyflieStatus::BthCharacteristicNotFound;
            }

            // CRTPUP Characteristic
            characteristicResults =
                co_await crazyflieService.GetCharacteristicsForUuidAsync(crtpUpCharacteristicGuid);
            if ((characteristicResults.Status() == GattCommunicationStatus::Success) &&
                (characteristicResults.Characteristics().Size()  == 1))
            {
                this->crtpUpCharacteristic_ =
                    std::make_shared<GattCharacteristic>(
                        characteristicResults.Characteristics().GetAt(0));
            }
            else
            {
                co_return CrazyflieStatus::BthCharacteristicNotFound;
            }

            // CRTPDOWN Characteristic
            characteristicResults =
                co_await crazyflieService.GetCharacteristicsForUuidAsync(crtpDownCharacteristicGuid);
            if ((characteristicResults.Status() == GattCommunicationStatus::Success) &&
                (characteristicResults.Characteristics().Size()  == 1))
            {
                this->crtpDownCharacteristic_ =
                    std::make_shared<GattCharacteristic>(
                        characteristicResults.Characteristics().GetAt(0));
            }
            else
            {
                co_return CrazyflieStatus::BthCharacteristicNotFound;
            }
        }
        else
        {
            co_return CrazyflieStatus::BthServiceNotFound;
        }

        // If we got this far, initialization was a success.
        this->initialized_ = true;
        return CrazyflieStatus::Success;
    }

    IAsyncOperation<CrazyflieStatus>
    BthDevice::SendAsync(
        CrtpPort port_id,
        IBuffer data)
    {
        DataWriter packet;
        CrtpHeader hdr =
        {
            0,
            0,
            static_cast<std::uint8_t>(port_id)
        };

        packet.WriteByte(hdr.as_byte);
        packet.WriteBuffer(data);

        auto res = co_await this->crtpCharacteristic_->WriteValueAsync(
            packet.DetachBuffer(),
            GattWriteOption::WriteWithResponse);

        return (res == GattCommunicationStatus::Success) ?
            CrazyflieStatus::Success : CrazyflieStatus::DeviceUnreachable;
    }
}