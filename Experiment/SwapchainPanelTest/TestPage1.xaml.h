#pragma once
#include "TestPage1.g.h"

#include "DeviceResources.h"

namespace winrt::SwapchainPanelTest::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::SizeChangedEventArgs;
using Microsoft::UI::Xaml::Input::TappedRoutedEventArgs;
using Windows::Foundation::IInspectable;

struct TestPage1 : TestPage1T<TestPage1> {
private:
  winrt::com_ptr<ISwapChainPanelNative> native = nullptr;
  DeviceResources* device_resources = nullptr;

public:
  TestPage1();

  void Clear();

  void use(DeviceResources*) noexcept;
  void on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const&);
  void on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&);
};
} // namespace winrt::SwapchainPanelTest::implementation

namespace winrt::SwapchainPanelTest::factory_implementation {
struct TestPage1 : TestPage1T<TestPage1, implementation::TestPage1> {};
} // namespace winrt::SwapchainPanelTest::factory_implementation
