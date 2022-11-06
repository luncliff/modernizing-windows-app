// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "HomePage.g.h"

namespace winrt::NavigationCpp::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::NavigationCpp::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
