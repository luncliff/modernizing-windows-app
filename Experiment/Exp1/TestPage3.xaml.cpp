#include "pch.h"

#include "TestPage3.xaml.h"
#if __has_include("TestPage3.g.cpp")
#include "TestPage3.g.cpp"
#endif
#include "MainWindow.xaml.h"
// #include <DirectXTK/Effects.h>

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include "StepTimer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Dxva2.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplay.lib")

namespace winrt::Exp1::implementation {
using namespace Microsoft::UI::Xaml;
using Windows::Storage::Streams::InputStreamOptions;

TestPage3::TestPage3() {
  InitializeComponent();
  SwapChainPanel panel = SwapchainPanel1();
  SetSwapChainPanel(panel);
}

TestPage3::~TestPage3() {
}

void TestPage3::OnNavigatedTo(const NavigationEventArgs& e) {
  spdlog::info("{}: {}", "TestPage3", __func__);
  auto address = winrt::unbox_value<uintptr_t>(e.Parameter());
  spdlog::debug("{}: {:p}", "TestPage3", reinterpret_cast<void*>(address));
  // cast to MainWindow and use getters to share objects
  auto window = reinterpret_cast<implementation::MainWindow*>(address);
  if (window == nullptr)
    return;
  dxgi = window->get_dxgi_provider();
  devices = window->get_device_provider();
  if (dxgi == nullptr || devices == nullptr)
    throw winrt::hresult_invalid_argument{};
}

void TestPage3::OnNavigatedFrom(const NavigationEventArgs&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
}

void TestPage3::Page_Loaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
  auto device = devices->get_dx11_device();
  auto adapter = dxgi->get_adapter();
  if (auto hr = MFCreateDXGIDeviceManager(&dxgi_manager_token, dxgi_manager.put()); FAILED(hr)) {
    spdlog::error("{}: {}", "TestPage3", winrt::hresult_error{hr});
    winrt::throw_hresult(hr);
  }
  // IID_ID3D11VideoDevice
  const GUID service_id = {0x10EC4D5B, 0x975A, 0x4689, {0xB9, 0xE4, 0xD0, 0xAA, 0xC3, 0x0F, 0xE3, 0x33}};
  if (auto hr = dxgi_manager->GetVideoService(dxgi_handle, service_id, video_device.put_void()); FAILED(hr)) {
    switch (hr) {
    case MF_E_DXGI_NEW_VIDEO_DEVICE:
      spdlog::debug("{}: {}", "TestPage3", "CloseDeviceHandle");
      dxgi_manager->CloseDeviceHandle(dxgi_handle);
      [[fallthrough]];
    case MF_E_DXGI_DEVICE_NOT_INITIALIZED:
    case E_HANDLE:
      spdlog::debug("{}: {}", "TestPage3", std::system_category().message(hr));
      break;
    default:
      winrt::throw_hresult(hr);
    }
  }
  spdlog::debug("{}: {}", "TestPage3", "ResetDevice");
  if (auto hr = dxgi_manager->ResetDevice(device.get(), dxgi_manager_token); FAILED(hr))
    winrt::throw_hresult(hr);
  spdlog::debug("{}: {}", "TestPage3", "OpenDeviceHandle");
  if (auto hr = dxgi_manager->OpenDeviceHandle(&dxgi_handle); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage3::Page_Unloaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
  if (dxgi_handle) {
    dxgi_manager->CloseDeviceHandle(dxgi_handle);
    dxgi_handle = 0;
  }
  dxgi_manager = nullptr;
  dxgi_manager_token = 0;
  video_device = nullptr;
}

void TestPage3::SetSwapChainPanel(SwapChainPanel panel) {
  IUnknown* unknown = winrt::get_unknown(panel);
  if (unknown == nullptr)
    winrt::throw_hresult(E_INVALIDARG);
  if (auto hr = unknown->QueryInterface(bridge.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage3::update_swapchain(Windows::Foundation::Size size) noexcept(false) {
  auto device = devices->get_dx11_device();

  DXGI_SWAP_CHAIN_DESC1 desc{};
  desc.Width = static_cast<UINT>(size.Width);
  desc.Height = static_cast<UINT>(size.Height);
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

  // if there is no swapchain, we have to create a new one
  if (swapchain == nullptr) {
    desc.Stereo = false;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 3;
    // All Windows Store apps must use this SwapEffect.
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.Flags = 0;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    if (auto hr = dxgi->create_swapchain(desc, device.get(), swapchain.put()); FAILED(hr)) {
      winrt::hresult_error ex{hr};
      spdlog::warn("{}: {} {}", "TestPage3", "swapchain create", ex);
      throw ex;
    }
    if (auto hr = bridge->SetSwapChain(swapchain.get()); FAILED(hr)) {
      winrt::hresult_error ex{hr};
      spdlog::warn("{}: {} {}", "TestPage3", "swapchain change", ex);
      throw ex;
    }
  }
  // if exists, change size
  else {
    if (auto hr = swapchain->ResizeBuffers(2, desc.Width, desc.Height, desc.Format, 0); FAILED(hr)) {
      winrt::hresult_error ex{hr};
      spdlog::warn("{}: {} {}", "TestPage3", "swapchain resize", ex);
      throw ex;
    }
    // print some messages for debugging
    auto msg = winrt::hstring{fmt::format(L"{:.1f} {:.1f} {:.3f}", //
                                          size.Width, size.Height, size.Width / size.Height)};
    spdlog::info("{}: {} {}", "TestPage3", "resized", msg);
  }
  // UpdateWindowSizeDependentResources...
  update_render_target(device.get());
}

void TestPage3::on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const& e) {
  if (devices == nullptr)
    return; // can't do the work...

  // remove references to the swapchain
  {
    auto device_context = devices->get_dx11_device_context();
    std::array<ID3D11RenderTargetView*, 1> targets0{};
    // no depth, stencil
    device_context->OMSetRenderTargets(static_cast<UINT>(targets0.size()), targets0.data(), nullptr);
  }
  render_target = nullptr;
  render_target_texture = nullptr;
  update_swapchain(e.NewSize());
}

void TestPage3::update_render_target(ID3D11Device* device) {
  render_target_texture = nullptr;
  if (auto hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), render_target_texture.put_void()); FAILED(hr))
    winrt::throw_hresult(hr);
  render_target = nullptr;
  if (auto hr = device->CreateRenderTargetView(render_target_texture.get(), 0, render_target.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage3::on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&) {
  D3D11_TEXTURE2D_DESC desc{};
  render_target_texture->GetDesc(&desc);
  spdlog::debug("{}: {:X}", "TestPage3", desc.CPUAccessFlags);
}

void TestPage3::SwapchainPanel1_PointerEntered(IInspectable const&, PointerRoutedEventArgs const&) {
  spdlog::debug("{}: mouse {}", "TestPage3", "entered");
}

void TestPage3::SwapchainPanel1_PointerMoved(IInspectable const&, PointerRoutedEventArgs const& e) {
  // acquire relative position to `nullptr`
  Microsoft::UI::Input::PointerPoint point = e.GetCurrentPoint(nullptr);
  const auto pos = point.Position();
  spdlog::trace("{}: mouse {} {:.2f} {:.2f}", "TestPage3", "moved", //
                pos.X, pos.Y);
}

void TestPage3::SwapchainPanel1_PointerExited(IInspectable const&, PointerRoutedEventArgs const&) {
  spdlog::debug("{}: mouse {}", "TestPage3", "exited");
}

} // namespace winrt::Exp1::implementation
