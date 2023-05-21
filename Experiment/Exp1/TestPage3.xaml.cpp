#include "pch.h"

#include "TestPage3.xaml.h"
#if __has_include("TestPage3.g.cpp")
#include "TestPage3.g.cpp"
#endif
#include "MainWindow.xaml.h"

#include <DirectXTK/DirectXHelpers.h>

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
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;
using Windows::Storage::Streams::InputStreamOptions;

struct VertexPositionTex {
  XMFLOAT3 pos;
  XMFLOAT2 tex;
};

struct QuadModelConstantBuffer {

  XMFLOAT4X4 model;
  XMFLOAT2 texCoordScale;
  XMFLOAT2 texCoordOffset;
};

TestPage3::TestPage3() {
  InitializeComponent();
  SwapChainPanel panel = SwapchainPanel1();
  SetSwapChainPanel(panel);
}

TestPage3::~TestPage3() {
  bridge = nullptr;
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

HRESULT TestPage3::create_nv12_texture(ID3D11Device* device) {
  CD3D11_TEXTURE2D_DESC desc{DXGI_FORMAT_NV12,      1280, 720, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC,
                             D3D11_CPU_ACCESS_WRITE};
  if (auto hr = device->CreateTexture2D(&desc, nullptr, nv12_texture.put()); FAILED(hr))
    return hr;

  CD3D11_SHADER_RESOURCE_VIEW_DESC desc0{nv12_texture.get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8_UNORM};
  if (auto hr = device->CreateShaderResourceView(nv12_texture.get(), &desc0, nv12_luma.put()); FAILED(hr))
    return hr;
  CD3D11_SHADER_RESOURCE_VIEW_DESC desc1{nv12_texture.get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8_UNORM};
  return device->CreateShaderResourceView(nv12_texture.get(), &desc1, nv12_chroma.put());
}

void TestPage3::cleanup_nv12_texture() {
  nv12_luma = nullptr;
  nv12_chroma = nullptr;
  nv12_texture = nullptr;
}

HRESULT TestPage3::update_nv12_texture([[maybe_unused]] IMFSample* sample) {
  if (devices == nullptr)
    return S_OK;
  spdlog::debug("{}: {}", "TestPage3", __func__);
  auto device_context = devices->get_dx11_device_context();

  DirectX::MapGuard guard{device_context.get(), nv12_texture.get(), 0, D3D11_MAP_WRITE_DISCARD, 0};
  void* ptr0 = guard.get(0);

  static uint8_t value = 0;
  if (++value > 250)
    value = 0;
  std::memset(ptr0, value, 1280 * 720);
  void* ptr1 = guard.get(1);
  return S_OK;
}

fire_and_forget TestPage3::update_resources() noexcept(false) {
  auto device = devices->get_dx11_device();
  auto device_context = devices->get_dx11_device_context();

  if (nv12_vs == nullptr) {
    // "main", "vs_5_0"
    StorageFile vsfile = co_await StorageFile::GetFileFromApplicationUriAsync(Uri{L"ms-appx:///QuadVertexShader.cso"});
    Windows::Storage::Streams::Buffer buf = nullptr;
    co_await read_cso(vsfile, buf);
    if (auto hr = device->CreateVertexShader(buf.data(), buf.Length(), nullptr, nv12_vs.put()); FAILED(hr)) {
      spdlog::error("{}: {} file {}", __func__, "CreateVertexShader", vsfile.Path());
      winrt::throw_hresult(hr);
    }
    // input layout
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> elements{{
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionTex, pos), D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPositionTex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0},
    }};
    device->CreateInputLayout(elements.data(), elements.size(), buf.data(), buf.Length(), layout.put());
  }

  if (nv12_ps == nullptr) {
    // "main", "ps_5_0"
    StorageFile psfile =
        co_await StorageFile::GetFileFromApplicationUriAsync(Uri{L"ms-appx:///QuadPixelShaderNV12.cso"});
    Windows::Storage::Streams::Buffer buf = nullptr;
    co_await read_cso(psfile, buf);
    // create shader
    if (auto hr = device->CreatePixelShader(buf.data(), buf.Length(), nullptr, nv12_ps.put()); FAILED(hr)) {
      spdlog::error("{}: {} file {}", __func__, "CreatePixelShader", psfile.Path());
      winrt::throw_hresult(hr);
    }
  }

  if (const_buffer == nullptr) {
    CD3D11_BUFFER_DESC desc{sizeof(QuadModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER};
    QuadModelConstantBuffer data{};
    data.texCoordScale = {1.0f, 1.0f};
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = &data;
    device->CreateBuffer(&desc, &init, const_buffer.put());
  }

  if (vertex_buffer == nullptr) {
    static const std::array<VertexPositionTex, 4> vertices{{
        {XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT2(0.f, 0.f)},
        {XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT2(1.f, 0.f)},
        {XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT2(1.f, 1.f)},
        {XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT2(0.f, 1.f)},
    }};
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(VertexPositionTex) * vertices.size();
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = vertices.data();
    device->CreateBuffer(&desc, &init, vertex_buffer.put());
  }

  if (index_buffer == nullptr) {
    // clang-format off
    static const std::array<uint16_t, 12> indices{{
        // -z
        0,2,3,
        0,1,2,
        // +z
        2,0,3,
        1,0,2,
    }};
    // clang-format on
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(uint16_t) * indices.size();
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = indices.data();
    device->CreateBuffer(&desc, &init, index_buffer.put());
  }

  if (sampler == nullptr) {
    CD3D11_SAMPLER_DESC desc{CD3D11_DEFAULT{}};
    device->CreateSamplerState(&desc, sampler.put());
  }
}

using namespace Windows::Foundation::Numerics;
inline float3 cast(const XMFLOAT3& v) {
  return *reinterpret_cast<const float3*>(&v);
};
inline XMFLOAT4X4 cast(const float4x4& m) {
  return *reinterpret_cast<const XMFLOAT4X4*>(&m);
};

void TestPage3::UpdateOnce() {
  if (devices == nullptr) {
    spdlog::warn("{}: {}", "TestPage3", __func__);
    return;
  }
  spdlog::debug("{}: {}", "TestPage3", __func__);
  auto device_context = devices->get_dx11_device_context();
  {
    // update constant buffers
    XMFLOAT3 position{}, normal{}, upvec{0, 0, 1};
    QuadModelConstantBuffer data{};
    data.model = cast(make_float4x4_scale(0.2f) * make_float4x4_world(cast(position), cast(normal), cast(upvec)));
    data.texCoordScale = {1.0f, 1.0f};
  }

  {
    std::array<ID3D11Buffer*, 1> iabuffers{vertex_buffer.get()};
    std::array<UINT, 1> strides{sizeof(VertexPositionTex)};
    std::array<UINT, 1> offsets{0};
    std::array<ID3D11ShaderResourceView*, 2> resviews{nv12_luma.get(), nv12_chroma.get()};
    std::array<ID3D11RenderTargetView*, 1> targets{render_target.get()};
    std::array<ID3D11SamplerState*, 1> samplers{sampler.get()};
    std::array<ID3D11Buffer*, 1> constants{const_buffer.get()};
    XMFLOAT4 blending{};

    device_context->OMSetBlendState(nullptr, reinterpret_cast<float*>(&blending), 0xffffffff);
    device_context->OMSetRenderTargets(targets.size(), targets.data(), nullptr);

    device_context->PSSetShader(nv12_ps.get(), nullptr, 0);
    device_context->PSSetShaderResources(0, resviews.size(), resviews.data());
    device_context->PSSetSamplers(0, samplers.size(), samplers.data());

    device_context->IASetVertexBuffers(0, iabuffers.size(), iabuffers.data(), strides.data(), offsets.data());
    device_context->IASetIndexBuffer(index_buffer.get(), DXGI_FORMAT_R16_UINT, 0);
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    device_context->IASetInputLayout(layout.get());
    device_context->VSSetShader(nv12_vs.get(), nullptr, 0);
    device_context->VSSetConstantBuffers(0, constants.size(), constants.data());
  }
  device_context->DrawIndexedInstanced(12, 2, 0, 0, 0);
}

HRESULT TestPage3::update_video_device(ID3D11Device* device) {
  if (auto hr = MFCreateDXGIDeviceManager(&dxgi_manager_token, dxgi_manager.put()); FAILED(hr)) {
    spdlog::error("{}: {}", "TestPage3", winrt::hresult_error{hr});
    return hr;
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
      return hr;
    }
  }
  spdlog::debug("{}: {}", "TestPage3", "ResetDevice");
  if (auto hr = dxgi_manager->ResetDevice(device, dxgi_manager_token); FAILED(hr))
    return hr;
  spdlog::debug("{}: {}", "TestPage3", "OpenDeviceHandle");
  if (auto hr = dxgi_manager->OpenDeviceHandle(&dxgi_handle); FAILED(hr))
    return hr;
  return S_OK;
}

void TestPage3::cleanup_video_device() {
  spdlog::info("{}: {}", "TestPage3", __func__);
  if (dxgi_handle) {
    dxgi_manager->CloseDeviceHandle(dxgi_handle);
    dxgi_handle = 0;
  }
  dxgi_manager = nullptr;
  dxgi_manager_token = 0;
  video_device = nullptr;
}

void TestPage3::Page_Loaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
  update_resources();
  auto device = devices->get_dx11_device();
  if (auto hr = update_video_device(device.get()); FAILED(hr)) {
    spdlog::error("{}: {} {}", "TestPage3", "update_video_device", winrt::hresult_error{hr});
    winrt::throw_hresult(hr);
  }
  if (auto hr = create_nv12_texture(device.get()); FAILED(hr)) {
    spdlog::error("{}: {} {}", "TestPage3", "create_nv12_texture", winrt::hresult_error{hr});
    winrt::throw_hresult(hr);
  }
}

void TestPage3::Page_Unloaded(IInspectable const&, RoutedEventArgs const&) {
  if (action_update == nullptr)
    return;
  try {
    action_update.Cancel();
    action_update.GetResults();
  } catch (const winrt::hresult_error& ex) {
    spdlog::warn("{}: {}", "TestPage3", ex);
  }
  action_update = nullptr;
  cleanup_video_device();
}

void TestPage3::on_step_timer_update(TestPage3* self) noexcept {
  self->update_nv12_texture(nullptr);
  self->UpdateOnce();
  if (auto hr = self->swapchain->Present(1, 0); FAILED(hr))
    spdlog::warn("{}: {}", "swapchain present", winrt::hresult_error{hr});
}

Windows::Foundation::IAsyncAction TestPage3::RunUpdate() {
  using namespace std::chrono_literals;

  auto token = co_await winrt::get_cancellation_token();
  co_await winrt::resume_background();

  StepTimer step{};
  step.SetFixedTimeStep(true);
  step.SetTargetElapsedSeconds(0.2f); // call update per 0.2 second
  try {
    while (token() == false) {
      std::this_thread::sleep_for(100ms);
      StepTimer::LPUPDATEFUNC updater = reinterpret_cast<StepTimer::LPUPDATEFUNC>(&TestPage3::on_step_timer_update);
      step.Tick(updater, this);
    }
    spdlog::debug("{}: {}", "TestPage3", "canceled");
  } catch (const winrt::hresult_error& ex) {
    const auto msg = ex.message();
    spdlog::error("{}: {}", "TestPage3", msg);
  }
}

void TestPage3::CancelUpdate() {
  if (action_update == nullptr)
    return;
  action_update.Cancel();
  action_update = nullptr;
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

  auto device_context = devices->get_dx11_device_context();
  // remove references to the swapchain
  std::array<ID3D11RenderTargetView*, 1> targets0{};
  // no depth, stencil
  device_context->OMSetRenderTargets(static_cast<UINT>(targets0.size()), targets0.data(), nullptr);

  render_target = nullptr;
  render_target_texture = nullptr;
  const auto size = e.NewSize();
  update_swapchain(size);

  CD3D11_VIEWPORT viewport{0.0f, 0.0f, static_cast<float>(size.Width), static_cast<float>(size.Height)};
  device_context->RSSetViewports(1, &viewport);
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

void TestPage3::ToggleSwitch0_Toggled(IInspectable const& s, RoutedEventArgs const&) {
  auto sender = s.as<Microsoft::UI::Xaml::Controls::ToggleSwitch>();
  if (sender == nullptr)
    return;
  if (sender.IsOn()) {
    action_update = RunUpdate();
    return;
  }
  CancelUpdate();
}

} // namespace winrt::Exp1::implementation
