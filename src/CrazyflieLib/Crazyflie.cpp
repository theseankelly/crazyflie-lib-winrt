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

    IAsyncOperation<CrazyflieStatus>
    Crazyflie::InitializeAsync()
    {
        this->bluetooth_device_ =
            std::make_shared<BthDevice>(this->device_name_);

        return this->bluetooth_device_->InitializeAsync();
    }

    IAsyncOperation<CrazyflieStatus>
    Crazyflie::SendCommanderPacketAsync(
        float roll,
        float pitch,
        float yaw,
        float thrust)
    {
        if ((roll < -1) || roll > 1)
        {
            throw winrt::hresult_error(
                E_INVALIDARG,
                L"Roll value must be a percentage between -1 and 1");
        }

        if ((pitch < -1) || pitch > 1)
        {
            throw winrt::hresult_error(
                E_INVALIDARG,
                L"Pitch value must be a percentage between -1 and 1");
        }

        if ((yaw < -1) || yaw > 1)
        {
            throw winrt::hresult_error(
                E_INVALIDARG,
                L"Yaw value must be a percentage between -1 and 1");
        }

        if ((thrust < 0) || thrust > 1)
        {
            throw winrt::hresult_error(
                E_INVALIDARG,
                L"Thrust value must be a percentage between 0 and 1");
        }

        DataWriter payload;
        payload.ByteOrder(ByteOrder::LittleEndian);
        payload.WriteSingle(roll * Crazyflie::MAX_ROLL);
        payload.WriteSingle(pitch * Crazyflie::MAX_PITCH);
        payload.WriteSingle(yaw * Crazyflie::MAX_YAW);
        payload.WriteUInt16(static_cast<std::uint16_t>(thrust * Crazyflie::MAX_THRUST));

        return this->bluetooth_device_->SendAsync(
            CrtpPort::Commander, payload.DetachBuffer());
    }

    IAsyncOperation<CrazyflieStatus>
    Crazyflie::SendCommanderHoverPacketAsync(
        float vx,
        float vy,
        float yawrate,
        float zdist)
    {
        DataWriter payload;
        payload.ByteOrder(ByteOrder::LittleEndian);
        payload.WriteByte(static_cast<std::uint8_t>(CrtpGenericSetpointId::Hover));
        payload.WriteSingle(vx);
        payload.WriteSingle(vy);
        payload.WriteSingle(yawrate);
        payload.WriteSingle(zdist);

        return this->bluetooth_device_->SendAsync(
            CrtpPort::GenericSetpoint, payload.DetachBuffer());
    }

    IAsyncOperation<CrazyflieStatus>
    Crazyflie::SendCommanderStopPacketAsync()
    {
        DataWriter payload;
        payload.ByteOrder(ByteOrder::LittleEndian);
        payload.WriteByte(static_cast<std::uint8_t>(CrtpGenericSetpointId::Stop));

        return this->bluetooth_device_->SendAsync(
            CrtpPort::GenericSetpoint, payload.DetachBuffer());
    }
}
