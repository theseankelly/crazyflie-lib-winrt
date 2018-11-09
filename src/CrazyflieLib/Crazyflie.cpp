#include "pch.h"
#include "Crazyflie.h"

namespace winrt::bitcraze::crazyflielib::implementation
{
    int32_t Crazyflie::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void Crazyflie::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
