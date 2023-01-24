#pragma once
#include "App.xaml.g.h"

#include "DeviceProvider.h"

namespace winrt::SwapchainPanelTest::implementation {
using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
using Microsoft::UI::Xaml::UnhandledExceptionEventArgs;
using Microsoft::UI::Xaml::Window;
using Microsoft::UI::Xaml::Controls::Page;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct App : AppT<App> {
  App();

  void OnLaunched(LaunchActivatedEventArgs const&);
  void on_unhandled_exception(IInspectable const&,
                              UnhandledExceptionEventArgs const& e);

private:
  DXGIProvider dxgi{};
  DeviceProvider devices{dxgi};

  Window window = nullptr;
};

} // namespace winrt::SwapchainPanelTest::implementation
