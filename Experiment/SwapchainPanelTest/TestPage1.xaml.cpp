#include "pch.h"

#include "TestPage1.xaml.h"
#if __has_include("TestPage1.g.cpp")
#include "TestPage1.g.cpp"
#endif

namespace winrt::SwapchainPanelTest::implementation {
using namespace Microsoft::UI::Xaml;

TestPage1::TestPage1() {
  InitializeComponent();
  auto panel = SwapchainPanel1();
  winrt::com_ptr<IUnknown> unknown = nullptr;
  if (auto hr = panel.as(__uuidof(IUnknown), unknown.put_void()); FAILED(hr))
    winrt::throw_hresult(hr);
  if (auto hr = unknown->QueryInterface(native.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage1::use(DeviceResources* ptr) noexcept {
  if (ptr == nullptr)
    throw winrt::hresult_invalid_argument{};
  device_resources = ptr;
  device_resources->SetSwapChainPanel(SwapchainPanel1());
}

void TestPage1::PausePage() {
  // ...
}

void TestPage1::ResumePage() {
  // ...
}

void TestPage1::on_panel_size_changed(IInspectable const&,
                                      SizeChangedEventArgs const& e) {
  auto s = e.NewSize();
  spdlog::info("{}: width {:.3f} height {:.3f}", "TestPage1", s.Width,
               s.Height);
  device_resources->UpdateWindowSizeDependentResources(s);
  if (auto hr = native->SetSwapChain(device_resources->swapchain.get());
      FAILED(hr))
    winrt::throw_hresult(hr);
}

} // namespace winrt::SwapchainPanelTest::implementation
