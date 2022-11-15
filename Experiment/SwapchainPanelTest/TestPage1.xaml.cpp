#include "pch.h"

#include "TestPage1.xaml.h"
#if __has_include("TestPage1.g.cpp")
#include "TestPage1.g.cpp"
#endif
// #include <DirectXTK/Effects.h>

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

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
  {
    auto panel = SwapchainPanel1();
    winrt::com_ptr<IUnknown> unknown = nullptr;
    if (auto hr = panel.as(__uuidof(IUnknown), unknown.put_void()); FAILED(hr))
      winrt::throw_hresult(hr);
    if (auto hr = unknown->QueryInterface(native.put()); FAILED(hr))
      winrt::throw_hresult(hr);
  }
}

IAsyncAction TestPage1::shaderTextBlock_Tapped(IInspectable const& s,
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

void TestPage1::use(DeviceResources* ptr) noexcept {
  if (ptr == nullptr)
    throw winrt::hresult_invalid_argument{};
  device_resources = ptr;
  device_resources->SetSwapChainPanel(SwapchainPanel1());
}

void TestPage1::Clear() {
  auto context = device_resources->device_context;
  auto render_target = device_resources->render_target;

  float color[4]{0, 0, 0, 0};
  context->ClearRenderTargetView(render_target.get(), color);
  context->OMSetRenderTargets(1, render_target.put(), nullptr);

  // auto viewport = device_resource->GetScreenViewport();
  // context->RSSetViewports(1, &viewport);
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

void TestPage1::on_panel_tapped(IInspectable const&,
                                TappedRoutedEventArgs const&) {
  // clear ...
  // present ...
}

} // namespace winrt::SwapchainPanelTest::implementation
