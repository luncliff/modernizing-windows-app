#include "pch.h"

#include "DeviceResources.h"

namespace winrt::SwapchainPanelTest {

DeviceResources::DeviceResources() noexcept(false) {
  UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED |
               D3D11_CREATE_DEVICE_BGRA_SUPPORT |
               D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#if defined(_DEBUG)
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
  D3D_FEATURE_LEVEL levels[]{D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
                             D3D_FEATURE_LEVEL_11_0};
  if (auto hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                  flags, levels, 3, D3D11_SDK_VERSION,
                                  device.put(), &level, device_context.put());
      FAILED(hr)) {
    spdlog::error("{}: {}", "DeviceResources",
                  winrt::to_string(winrt::hresult_error{hr}.message()));
    winrt::throw_hresult(hr);
  }
  if (level >= D3D_FEATURE_LEVEL_12_0) {
    winrt::com_ptr<ID3D12Device> device12 = nullptr;
    // ...
  }
  UpdateDevice(device, device_context);
}

DeviceResources::~DeviceResources() noexcept {
  // spdlog::warn("{}: {}", "DeviceResources", "Releasing DirectX resources");
}

void DeviceResources::SetSwapChainPanel(SwapChainPanel panel) noexcept(false) {
  IUnknown* unknown = winrt::get_unknown(panel);
  if (unknown == nullptr)
    winrt::throw_hresult(E_INVALIDARG);
  bridge = nullptr;
  if (auto hr = unknown->QueryInterface(bridge.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void DeviceResources::UpdateDevice(
    winrt::com_ptr<ID3D11Device> d,
    winrt::com_ptr<ID3D11DeviceContext> c) noexcept(false) {
  if (d == nullptr) {
    spdlog::warn(
        "{}: {}", "DeviceResources",
        "no implementation. this function should handle device lost situation");
    return;
  }
  // update DirectX device...
  device = d;
  if (device_context != nullptr)
    device_context = c;
  else {
    device_context = nullptr;
    device->GetImmediateContext(device_context.put());
  }
  // update DXGI resources...
  winrt::com_ptr<IDXGIDevice2> dxgi{};
  if (auto hr = this->device->QueryInterface(dxgi.put()); FAILED(hr))
    winrt::throw_hresult(hr);
  winrt::com_ptr<IDXGIAdapter> adapter{};
  if (auto hr = dxgi->GetParent(__uuidof(IDXGIAdapter), adapter.put_void());
      FAILED(hr))
    winrt::throw_hresult(hr);
  if (auto hr =
          adapter->GetParent(__uuidof(IDXGIFactory2), dxgi_factory.put_void());
      FAILED(hr))
    winrt::throw_hresult(hr);
}

void DeviceResources::UpdateRenderTarget() noexcept(false) {
  render_target_texture = nullptr;
  if (auto hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                     render_target_texture.put_void());
      FAILED(hr))
    winrt::throw_hresult(hr);
  render_target = nullptr;
  if (auto hr = device->CreateRenderTargetView(render_target_texture.get(), 0,
                                               render_target.put());
      FAILED(hr))
    winrt::throw_hresult(hr);
}

void DeviceResources::UpdateWindowSizeDependentResources(
    Windows::Foundation::Size size) noexcept(false) {
  ID3D11RenderTargetView* targets0[1]{nullptr};
  device_context->OMSetRenderTargets(1, targets0, nullptr); // no depth, stencil

  // Match the size,format of the window...
  DXGI_SWAP_CHAIN_DESC1 desc{};
  desc.Width = static_cast<UINT>(size.Width);
  desc.Height = static_cast<UINT>(size.Height);
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

  // remove references to the swapchain
  render_target = nullptr;
  render_target_texture = nullptr;
  if (swapchain != nullptr) {
    // if exists, change its size
    if (auto hr = swapchain->ResizeBuffers(2, desc.Width, desc.Height,
                                           desc.Format, 0);
        FAILED(hr)) {
      winrt::hresult_error ex{hr};
      spdlog::warn("{}: {}", "DeviceResources", winrt::to_string(ex.message()));
      throw ex;
    }
    spdlog::debug("{}: {}", "DeviceResources", "resized swapchain");
  } else {
    // if there is no swapchain, we have to create a new one
    desc.Stereo = false;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2; // Use double-buffering to minimize latency.
    desc.SwapEffect =
        DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use
                                          // this SwapEffect.
    desc.Flags = 0;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    if (auto hr = dxgi_factory->CreateSwapChainForComposition(
            device.get(), &desc, nullptr, swapchain.put());
        FAILED(hr))
      winrt::throw_hresult(hr);
    spdlog::info("{}: {}", "DeviceResources", "created swapchain");

    if (auto hr = bridge->SetSwapChain(swapchain.get()); FAILED(hr))
      winrt::throw_hresult(hr);
    spdlog::info("{}: {}", "DeviceResources", "updated swapchain");
  }

  // OK. Acquire the render target handles
  UpdateRenderTarget();
  return;
}

void DeviceResources::SetCurrentOrientation(
    [[maybe_unused]] DisplayOrientations orientation) {
  // ...
}

void DeviceResources::SetCompositionScale([[maybe_unused]] float scaleX,
                                          [[maybe_unused]] float scaleY) {
  // ...
}

} // namespace winrt::SwapchainPanelTest