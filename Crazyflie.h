#pragma once

#include "Crazyflie.g.h"

namespace winrt::bitcraze::crazyflielib::implementation
{
    struct Crazyflie : CrazyflieT<Crazyflie>
    {
        Crazyflie() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::bitcraze::crazyflielib::factory_implementation
{
    struct Crazyflie : CrazyflieT<Crazyflie, implementation::Crazyflie>
    {
    };
}
