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

#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Dxva2.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplay.lib")

namespace winrt::Exp1::implementation {

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

  try {
    auto device = devices->get_dx11_device();
    auto device_context = devices->get_dx11_device_context();
    renderer = std::make_unique<NV12TextureRenderer>(device.get(), device_context.get());
  } catch (const winrt::hresult_error& ex) {
    spdlog::error("{}: {} {}", "TestPage3", "NV12TextureRenderer", ex);
  }
}

void TestPage3::OnNavigatedFrom(const NavigationEventArgs&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
  if (action_update == nullptr)
    return;
  CancelUpdate();
  if (auto context = devices->get_dx11_device_context(); context) {
    context->ClearState();
    context->Flush();
  }
  renderer = nullptr;
}

void TestPage3::Page_Loaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
}

void TestPage3::Page_Unloaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage3", __func__);
}

void TestPage3::on_step_timer_update(TestPage3* self) noexcept {
  auto renderer = self->renderer.get();
  renderer->update();
  ID3D11Texture2D* source = nullptr;
  renderer->update_texture(source);
  // render only if we have a render target
  auto render_target = self->render_target;
  if (render_target == nullptr)
    return;
  renderer->render(render_target.get());
}

Windows::Foundation::IAsyncAction TestPage3::RunUpdate() {
  auto token = co_await winrt::get_cancellation_token();
  co_await winrt::resume_background();

  // prepare rendering
  co_await renderer->setup_resources();
  renderer->create_texture(1280, 720);

  StepTimer step{};
  step.SetFixedTimeStep(true);
  step.SetTargetElapsedSeconds(0.2f); // call update per 0.2 second
  try {
    while (token() == false) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(10ms);

      StepTimer::LPUPDATEFUNC updater = reinterpret_cast<StepTimer::LPUPDATEFUNC>(&TestPage3::on_step_timer_update);
      step.Tick(updater, this);

      if (auto hr = swapchain->Present(1, 0); FAILED(hr))
        spdlog::warn("{}: {}", "TestPage3", winrt::hresult_error{hr});
    }
    spdlog::debug("{}: {}", "TestPage3", "canceled");
  } catch (const winrt::hresult_error& ex) {
    const auto msg = ex.message();
    spdlog::error("{}: {}", "TestPage3", msg);
  }
}

void TestPage3::CancelUpdate() {
  try {
    action_update.Cancel();
    action_update.GetResults();
  } catch (const winrt::hresult_error& ex) {
    spdlog::warn("{}: {}", "TestPage3", ex);
  }
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
