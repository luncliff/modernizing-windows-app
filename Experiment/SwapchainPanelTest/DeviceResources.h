#pragma once
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <concrt.h>
#include <d2d1_3.h>
#include <d3d11_4.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dwrite.h>
#include <dxgi1_6.h>
#include <pix.h>
#include <shellapi.h>

#include <winrt/Microsoft.Graphics.DirectX.h>
#include <winrt/Microsoft.UI.Composition.Interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>

#include <microsoft.ui.xaml.media.dxinterop.h> // NOT <windows.ui.xaml.media.dxinterop.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.h>
#include <windows.graphics.directx.direct3d11.interop.h>

namespace winrt::SwapchainPanelTest {

using Microsoft::UI::Xaml::Controls::SwapChainPanel;
using Windows::Graphics::Display::DisplayOrientations;

class DeviceResources {
public:
  winrt::com_ptr<ID3D11Device> device = nullptr;
  winrt::com_ptr<ID3D11DeviceContext> device_context = nullptr;
  winrt::com_ptr<IDXGIFactory2> dxgi_factory = nullptr;
  winrt::com_ptr<ISwapChainPanelNative> bridge = nullptr;
  winrt::com_ptr<IDXGISwapChain1> swapchain = nullptr;
  winrt::com_ptr<ID3D11RenderTargetView> render_target = nullptr;
  winrt::com_ptr<ID3D11Texture2D> render_target_texture = nullptr;

public:
  DeviceResources() noexcept(false);
  ~DeviceResources() noexcept;

  void SetSwapChainPanel(SwapChainPanel panel) noexcept(false);
  void SetCurrentOrientation(DisplayOrientations orientation);
  void SetCompositionScale(float scaleX, float scaleY);

  void UpdateDevice(winrt::com_ptr<ID3D11Device> device = nullptr,
                    winrt::com_ptr<ID3D11DeviceContext> device_context =
                        nullptr) noexcept(false);
  void UpdateWindowSizeDependentResources(
      Windows::Foundation::Size size) noexcept(false);

private:
  void UpdateRenderTarget() noexcept(false);
};

} // namespace winrt::SwapchainPanelTest