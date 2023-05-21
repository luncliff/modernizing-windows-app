#include "pch.h"

#include "TestPage1.xaml.h"
#if __has_include("TestPage1.g.cpp")
#include "TestPage1.g.cpp"
#endif
#include "MainWindow.xaml.h"
// #include <DirectXTK/Effects.h>

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include "StepTimer.h"

namespace winrt::Exp1::implementation {
using namespace Microsoft::UI::Xaml;
using Windows::Storage::Streams::InputStreamOptions;

struct ui_dispatcher_queue_awaiter_t {
  Microsoft::UI::Dispatching::DispatcherQueue queue;

  ui_dispatcher_queue_awaiter_t(Microsoft::UI::Dispatching::DispatcherQueue queue) : queue{queue} {
    // ...
  }

  constexpr bool await_ready() {
    return false;
  }
  constexpr void await_resume() {
  }
  bool await_suspend(std::experimental::coroutine_handle<void> coro) {
    return queue.TryEnqueue(coro);
  }
};

TestPage1::TestPage1() {
  InitializeComponent();
  foreground = Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
  SwapChainPanel panel = SwapchainPanel1();
  SetSwapChainPanel(panel);
}

TestPage1::~TestPage1() {
  if (devices)
    wait_for_gpu();
  if (d12_fence_event != INVALID_HANDLE_VALUE)
    CloseHandle(d12_fence_event);
}

void TestPage1::OnNavigatedTo(const NavigationEventArgs& e) {
  spdlog::info("{}: {}", "TestPage1", __func__);
  auto address = winrt::unbox_value<uintptr_t>(e.Parameter());
  spdlog::debug("{}: {:p}", "TestPage1", reinterpret_cast<void*>(address));

  auto window = reinterpret_cast<implementation::MainWindow*>(address);
  if (window == nullptr)
    return;

  dxgi = window->get_dxgi_provider();
  devices = window->get_device_provider();
  if (dxgi == nullptr || devices == nullptr)
    throw winrt::hresult_invalid_argument{};
}

void TestPage1::OnNavigatedFrom(const NavigationEventArgs&) {
  spdlog::info("{}: {}", "TestPage1", __func__);
}

/// @see after OnNavigatedTo, prepare some resources
void TestPage1::Page_Loaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage1", __func__);
  setup_graphics();
}

void TestPage1::Page_Unloaded(IInspectable const&, RoutedEventArgs const&) {
  spdlog::info("{}: {}", "TestPage1", __func__);
  if (action0 != nullptr) {
    action0.Cancel();
    action0 = nullptr;
  }
}

void TestPage1::setup_graphics() noexcept(false) {
  // event handle for frame synchronization
  d12_fence_event = CreateEventW(nullptr, false, false, nullptr);
  if (d12_fence_event == INVALID_HANDLE_VALUE)
    winrt::throw_last_error();
  // fence for command queue signal
  auto device = devices->get_dx12_device();
  if (auto hr = device->CreateFence(d12_fence_values[frame_index], D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),
                                    d12_fence.put_void());
      FAILED(hr))
    winrt::throw_hresult(hr);
  d12_fence_values[frame_index]++;
  wait_for_gpu();
}

void TestPage1::SetSwapChainPanel(SwapChainPanel panel) {
  IUnknown* unknown = winrt::get_unknown(panel);
  if (unknown == nullptr)
    winrt::throw_hresult(E_INVALIDARG);
  if (auto hr = unknown->QueryInterface(bridge.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage1::Clear() {
  if (devices == nullptr)
    return;
  if (render_target == nullptr)
    return;
  const float c = []() -> float {
    static float value = -1000.0f;
    value += 0.01f;
    return sinf(value) * 0.4f + 0.5f;
  }();
  float color[4]{0, 0, c, 1};
  auto device_context = devices->get_dx11_device_context();
  device_context->ClearRenderTargetView(render_target.get(), color);
}

void TestPage1::update_swapchain(Windows::Foundation::Size size) noexcept(false) {
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

void TestPage1::on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const& e) {
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

  // UpdateWindowSizeDependentResources...
  auto device = devices->get_dx11_device();
  const auto size = e.NewSize();
  update_swapchain(size);

  // print some messages for debugging
  auto msg = fmt::format(L"{:.1f} {:.1f} {:.3f}", size.Width, size.Height, size.Width / size.Height);
  update_description(winrt::hstring{msg});
}

void TestPage1::update_render_target(ID3D11Device* device) {
  render_target_texture = nullptr;
  if (auto hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), render_target_texture.put_void()); FAILED(hr))
    winrt::throw_hresult(hr);
  render_target = nullptr;
  if (auto hr = device->CreateRenderTargetView(render_target_texture.get(), 0, render_target.put()); FAILED(hr))
    winrt::throw_hresult(hr);
}

void TestPage1::on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&) {
  D3D11_TEXTURE2D_DESC desc{};
  render_target_texture->GetDesc(&desc);
  spdlog::debug("{}: {:X}", "TestPage1", desc.CPUAccessFlags);
}

void TestPage1::update_frame_index() noexcept(false) {
  // Schedule a Signal command in the queue.
  auto command_queue = devices->get_dx12_command_queue();
  auto fence_value = d12_fence_values[frame_index];
  if (auto hr = command_queue->Signal(d12_fence.get(), fence_value); FAILED(hr))
    winrt::throw_hresult(hr);

  frame_index = swapchain->GetCurrentBackBufferIndex();

  // If the next frame is not ready to be rendered yet, wait until it is ready.
  if (d12_fence->GetCompletedValue() < d12_fence_values[frame_index]) {
    if (auto hr = d12_fence->SetEventOnCompletion(d12_fence_values[frame_index], d12_fence_event); FAILED(hr))
      winrt::throw_hresult(hr);

    WaitForSingleObjectEx(d12_fence_event, INFINITE, FALSE);
  }

  // Set the fence value for the next frame.
  d12_fence_values[frame_index] = fence_value + 1;
}

void TestPage1::wait_for_gpu() noexcept(false) {
  // Schedule a Signal command in the queue.
  auto command_queue = devices->get_dx12_command_queue();
  auto fence_value = d12_fence_values[frame_index];
  if (auto hr = command_queue->Signal(d12_fence.get(), fence_value); FAILED(hr))
    winrt::throw_hresult(hr);

  // If the next frame is not ready to be rendered yet, wait until it is ready.
  if (d12_fence->GetCompletedValue() < d12_fence_values[frame_index]) {
    if (auto hr = d12_fence->SetEventOnCompletion(d12_fence_values[frame_index], d12_fence_event); FAILED(hr))
      winrt::throw_hresult(hr);

    WaitForSingleObjectEx(d12_fence_event, INFINITE, FALSE);
  }

  // Increment the fence value for the current frame.
  d12_fence_values[frame_index] += 1;
}

IAsyncAction TestPage1::StartUpdate() {
  using namespace std::chrono_literals;

  auto token = co_await winrt::get_cancellation_token();
  co_await winrt::resume_background();

  StepTimer step{};
  step.SetFixedTimeStep(true);
  step.SetTargetElapsedSeconds(1); // call update per 1 second
  try {
    while (token() == false) {
      std::this_thread::sleep_for(10ms);
      this->Clear();
      if (auto hr = swapchain->Present(1, 0); FAILED(hr))
        spdlog::warn("{}: {}", "swapchain present", winrt::hresult_error{hr});
      step.Tick([](void*) { spdlog::debug("{}: {}", "TestPage1", "tick"); });
    }
    spdlog::debug("{}: {}", "TestPage1", "canceled");
  } catch (const winrt::hresult_error& ex) {
    const auto msg = ex.message();
    spdlog::error("{}: {}", "TestPage1", msg);
    update_description(msg);
  }
}

void TestPage1::SwapchainPanel1_PointerEntered(IInspectable const&, PointerRoutedEventArgs const&) {
  spdlog::debug("{}: mouse {}", "TestPage1", "entered");
}

void TestPage1::SwapchainPanel1_PointerMoved(IInspectable const&, PointerRoutedEventArgs const& e) {
  // acquire relative position to `nullptr`
  Microsoft::UI::Input::PointerPoint point = e.GetCurrentPoint(nullptr);
  const auto pos = point.Position();
  spdlog::trace("{}: mouse {} {:.2f} {:.2f}", "TestPage1", "moved", //
                pos.X, pos.Y);
}

void TestPage1::SwapchainPanel1_PointerExited(IInspectable const&, PointerRoutedEventArgs const&) {
  spdlog::debug("{}: mouse {}", "TestPage1", "exited");
}

IAsyncAction TestPage1::validate_assets(IInspectable const&, TappedRoutedEventArgs const&) {
  Windows::Storage::StorageFile file = co_await Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(
      Windows::Foundation::Uri{L"ms-appx:///Assets/shaders.hlsl"});
  if (file == nullptr)
    co_return;

  auto stream = co_await file.OpenReadAsync();
  auto capacity = static_cast<uint32_t>(stream.Size());
  Windows::Storage::Streams::Buffer buf(capacity);

  co_await stream.ReadAsync(buf, capacity, InputStreamOptions::None);

  // std::wstring text =
  //     mb2w({reinterpret_cast<const char*>(buf.data()), buf.Length()});

  // co_await ui_dispatcher_queue_awaiter_t{foreground};
  // auto tb = shaderTextBlock();
  // tb.Text(text);
}

fire_and_forget TestPage1::update_description(winrt::hstring message) {
  co_await ui_dispatcher_queue_awaiter_t{foreground};
  auto text = descriptionText();
  text.Text(message);
}

fire_and_forget TestPage1::update_progress(float ratio) {
  co_await ui_dispatcher_queue_awaiter_t{foreground};
  auto bar = progressBar();
  ratio = std::clamp(ratio, 0.0f, 1.0f);
  bar.Value(ratio * bar.Maximum());
}

void TestPage1::updateSwitch_Toggled(IInspectable const& s, RoutedEventArgs const&) {
  auto sender = s.as<Microsoft::UI::Xaml::Controls::ToggleSwitch>();
  if (sender == nullptr)
    return;
  auto content = sender.IsOn() ? sender.OnContent() : sender.OffContent();
  auto text = winrt::unbox_value<winrt::hstring>(content);
  spdlog::debug("{}: {}", "TestPage1", text);
  if (swapchain == nullptr)
    return; // can't do the work...

  if (sender.IsOn()) {
    action0 = StartUpdate();
    return;
  }
  if (action0 != nullptr) {
    action0.Cancel();
    action0 = nullptr;
  }
}

} // namespace winrt::Exp1::implementation
