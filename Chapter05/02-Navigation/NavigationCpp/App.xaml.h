#pragma once

#include "App.xaml.g.h"

namespace winrt::NavigationCpp::implementation
{
    using Microsoft::UI::Xaml::Window;
    using Microsoft::UI::Xaml::Controls::Page;
    using Microsoft::UI::Xaml::LaunchActivatedEventArgs;

    struct App : AppT<App>
    {
        App();

        void OnLaunched(LaunchActivatedEventArgs const&);

    private:
        Window window{ nullptr };
    };
}
