#pragma once
#include "MainWindow.g.h"

namespace winrt::DispatcherCpp::implementation
{
using Microsoft::UI::Xaml::RoutedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct MainWindow : MainWindowT<MainWindow>
{
    MainWindow() noexcept = default;

    IAsyncAction OnUpdateMessage(IInspectable const &, RoutedEventArgs const &);
    IAsyncAction OnUpdateMessageWithToolkit(IInspectable const &, RoutedEventArgs const &);
};
} // namespace winrt::DispatcherCpp::implementation

namespace winrt::DispatcherCpp::factory_implementation
{
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
{
};
} // namespace winrt::DispatcherCpp::factory_implementation
