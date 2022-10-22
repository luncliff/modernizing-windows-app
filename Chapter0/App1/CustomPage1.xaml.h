#pragma once
#include "CustomPage1.g.h"

#include <d3d11_4.h>
#include <d3d11on12.h>
#include <dxgi1_6.h>
#include <winrt/Microsoft.Graphics.DirectX.h>

namespace winrt::App1::implementation {

using namespace Microsoft::UI::Xaml;

struct CustomPage1 : CustomPage1T<CustomPage1> {
  private:
    winrt::com_ptr<ISwapChainPanelNative> native = nullptr;
    winrt::com_ptr<IDXGISwapChain1> swapchain = nullptr;

  public:
    CustomPage1();

    void on_window_size_changed(IInspectable const&, WindowSizeChangedEventArgs const&);
};

} // namespace winrt::App1::implementation

namespace winrt::App1::factory_implementation {
struct CustomPage1 : CustomPage1T<CustomPage1, implementation::CustomPage1> {};
} // namespace winrt::App1::factory_implementation
