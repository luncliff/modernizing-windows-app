#pragma once
#include "MainWindow.g.h"

#include <winrt/Microsoft.UI.Xaml.h>

namespace winrt::App1::implementation {

using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::WindowSizeChangedEventArgs;
using Microsoft::UI::Xaml::WindowVisibilityChangedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct MainWindow : MainWindowT<MainWindow> {
  private:
    HWND hwnd = nullptr;
    Windows::System::DispatcherQueue queue = nullptr;

  public:
    MainWindow();

    Windows::System::DispatcherQueue BackgroundQueue() noexcept;
    void BackgroundQueue(Windows::System::DispatcherQueue queue) noexcept;

    IAsyncAction ChangeTheme();

    void on_button1_clicked(IInspectable const& sender, RoutedEventArgs const& args);
    void on_button2_clicked(IInspectable const& sender, RoutedEventArgs const& args);

    void on_window_size_changed(IInspectable const& sender, WindowSizeChangedEventArgs const& e);
    void on_window_visibility_changed(IInspectable const& sender, WindowVisibilityChangedEventArgs const& e);
};
} // namespace winrt::App1::implementation

namespace winrt::App1::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::App1::factory_implementation
