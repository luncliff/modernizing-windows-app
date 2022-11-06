#pragma once
#include "TestPage1.g.h"

#include "GPUResources.h"
#include <d3d11_4.h>
#include <d3d11on12.h>
#include <dxgi1_6.h>
#include <winrt/Microsoft.Graphics.DirectX.h>

// MIDL headers
#include <microsoft.ui.xaml.media.dxinterop.h>
#include <microsoft.ui.xaml.window.h>

namespace winrt::SwapchainPanelTest::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::SizeChangedEventArgs;
using Windows::Foundation::IInspectable;

struct TestPage1 : TestPage1T<TestPage1> {
private:
  winrt::com_ptr<ISwapChainPanelNative> native = nullptr;
  winrt::com_ptr<IDXGISwapChain1> swapchain = nullptr;
  GPUResources* gpu_resources = nullptr;

public:
  TestPage1();

  void PausePage();
  void ResumePage();

  void use(GPUResources*) noexcept;
  void on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const&);
};
} // namespace winrt::SwapchainPanelTest::implementation

namespace winrt::SwapchainPanelTest::factory_implementation {
struct TestPage1 : TestPage1T<TestPage1, implementation::TestPage1> {};
} // namespace winrt::SwapchainPanelTest::factory_implementation
