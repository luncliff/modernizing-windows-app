#include "pch.h"

#include "TestPage1.xaml.h"
#if __has_include("TestPage1.g.cpp")
#include "TestPage1.g.cpp"
#endif
// #include <DirectXTK/Effects.h>

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include "StepTimer.h"

namespace winrt::SwapchainPanelTest::implementation {
using namespace Microsoft::UI::Xaml;
using Windows::Storage::Streams::InputStreamOptions;

struct ui_dispatcher_queue_awaiter_t {
  Microsoft::UI::Dispatching::DispatcherQueue queue;

  ui_dispatcher_queue_awaiter_t(
      Microsoft::UI::Dispatching::DispatcherQueue queue)
      : queue{queue} {
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
  foreground =
      Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
  SwapChainPanel panel = SwapchainPanel1();
  SetSwapChainPanel(panel);
}
TestPage1::~TestPage1() {
  this->wait_for_gpu();
  if (d12_fence_event != INVALID_HANDLE_VALUE)
    CloseHandle(d12_fence_event);
}

void TestPage1::use(DXGIProvider* dxgi,
                    DeviceProvider* devices) noexcept(false) {
  if (dxgi == nullptr || devices == nullptr)
    throw winrt::hresult_invalid_argument{};
  this->dxgi = dxgi;
  this->devices = devices;

  // todo: use supports_dx12
  if (devices)
    return;

  // event handle for frame synchronization
  d12_fence_event = CreateEventW(nullptr, false, false, nullptr);
  if (d12_fence_event == INVALID_HANDLE_VALUE)
    winrt::throw_last_error();
  // fence for command queue signal
  auto device = devices->get_dx12_device();
  if (auto hr = device->CreateFence(
          d12_fence_values[frame_index], D3D12_FENCE_FLAG_NONE,
          __uuidof(ID3D12Fence), d12_fence.put_void());
      FAILED(hr))
    winrt::throw_hresult(hr);
  d12_fence_values[frame_index]++;

  return this->wait_for_gpu();
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
  // clear ...
  {
    const float c = []() -> float {
      static float value = 0;
      value += 0.01f;
      if (value >= 1)
        value = 0;
      return (sinf(value) / 2 + 0.5f); // [0,1)
    }();
    float color[4]{0, c, 0, 1};
    auto device_context = devices->get_dx11_device_context();
    device_context->ClearRenderTargetView(render_target.get(), color);
  }
  // present...
  {
    if (auto hr = swapchain->Present(1, 0); FAILED(hr))
      spdlog::warn("{}: {:X}", "swapchain present", static_cast<uint32_t>(hr));
  }
}

void TestPage1::on_panel_size_changed(IInspectable const&,
                                      SizeChangedEventArgs const& e) {
  // remove references to the swapchain
  devices->reset_render_targets();
  render_target = nullptr;
  render_target_texture = nullptr;

  auto device = devices->get_dx11_device();

  DXGI_SWAP_CHAIN_DESC1 desc{};
  auto size = e.NewSize();
  desc.Width = static_cast<UINT>(size.Width);
  desc.Height = static_cast<UINT>(size.Height);
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

  // if there is no swapchain, we have to create a new one
  if (swapchain == nullptr) {
    desc.Stereo = false;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = num_frames;
    // All Windows Store apps must use this SwapEffect.
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.Flags = 0;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    if (auto hr = dxgi->create_swapchain(desc, device.get(), swapchain.put());
        FAILED(hr)) {
      spdlog::warn("{}: {} {:X}", "TestPage1", "swapchain create",
                   static_cast<uint32_t>(hr));
      winrt::throw_hresult(hr);
    }
    if (auto hr = bridge->SetSwapChain(swapchain.get()); FAILED(hr)) {
      spdlog::warn("{}: {} {:X}", "TestPage1", "swapchain change",
                   static_cast<uint32_t>(hr));
      winrt::throw_hresult(hr);
    }
  }
  // if exists, change size
  else {
    if (auto hr = swapchain->ResizeBuffers(2, desc.Width, desc.Height,
                                           desc.Format, 0);
        FAILED(hr)) {
      winrt::hresult_error ex{hr};
      spdlog::warn("{}: {}", "TestPage1", winrt::to_string(ex.message()));
      throw ex;
    }
    spdlog::info("{}: {} {:.2f} {:.2f}", "TestPage1", "resized", //
                 size.Width, size.Height);
  }
  // UpdateWindowSizeDependentResources...
  return this->update_render_target(device.get());
}

void TestPage1::update_render_target(ID3D11Device* device) {
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

void TestPage1::on_panel_tapped(IInspectable const&,
                                TappedRoutedEventArgs const&) {
  this->Clear();
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
    if (auto hr = d12_fence->SetEventOnCompletion(d12_fence_values[frame_index],
                                                  d12_fence_event);
        FAILED(hr))
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
    if (auto hr = d12_fence->SetEventOnCompletion(d12_fence_values[frame_index],
                                                  d12_fence_event);
        FAILED(hr))
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

  while (token() == false) {
    std::this_thread::sleep_for(10ms);
    this->Clear();
    step.Tick([](void*) { spdlog::debug("{}: {}", "TestPage1", "tick"); });
  }
}

void TestPage1::SwapchainPanel1_PointerEntered(
    IInspectable const&, PointerRoutedEventArgs const& e) {
  spdlog::debug("{}: mouse {}", "TestPage1", "entered");
}

void TestPage1::SwapchainPanel1_PointerMoved(IInspectable const&,
                                             PointerRoutedEventArgs const& e) {
}

void TestPage1::SwapchainPanel1_PointerExited(IInspectable const&,
                                              PointerRoutedEventArgs const& e) {
  spdlog::debug("{}: mouse {}", "TestPage1", "exited");
}

IAsyncAction TestPage1::shaderTextBlock_Tapped(IInspectable const&,
                                               TappedRoutedEventArgs const&) {
  Windows::Storage::StorageFile file =
      co_await Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(
          Windows::Foundation::Uri{L"ms-appx:///Assets/shaders.hlsl"});
  if (file == nullptr)
    co_return;

  auto stream = co_await file.OpenReadAsync();
  auto capacity = static_cast<uint32_t>(stream.Size());
  Windows::Storage::Streams::Buffer buf(capacity);

  co_await stream.ReadAsync(buf, capacity, InputStreamOptions::None);
  std::wstring text =
      mb2w({reinterpret_cast<const char*>(buf.data()), buf.Length()});

  co_await ui_dispatcher_queue_awaiter_t{foreground};
  auto tb = shaderTextBlock();
  tb.Text(text);
}

void TestPage1::updateToggle_Toggled(IInspectable const& s,
                                     RoutedEventArgs const& e) {
  auto sender = s.as<Microsoft::UI::Xaml::Controls::ToggleSwitch>();
  if (sender == nullptr)
    return;
  auto content = sender.IsOn() ? sender.OnContent() : sender.OffContent();
  auto text = winrt::unbox_value<winrt::hstring>(content);
  spdlog::debug("{}: {}", "TestPage1", winrt::to_string(text));

  if (sender.IsOn()) {
    action0 = StartUpdate();
  } else if (action0 != nullptr) {
    action0.Cancel();
  }
}

} // namespace winrt::SwapchainPanelTest::implementation
