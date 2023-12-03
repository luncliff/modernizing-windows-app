#pragma once

#include "MainWindow.g.h"

namespace winrt::PowerCpp::implementation
{
struct MainWindow : MainWindowT<MainWindow>
{
    MainWindow();
};
} // namespace winrt::PowerCpp::implementation

namespace winrt::PowerCpp::factory_implementation
{
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
{
};
} // namespace winrt::PowerCpp::factory_implementation
