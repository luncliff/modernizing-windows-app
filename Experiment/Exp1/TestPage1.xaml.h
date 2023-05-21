#pragma once
#include "TestPage1.g.h"

#include "DeviceProvider.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::Exp1::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::SizeChangedEventArgs;
using Microsoft::UI::Xaml::Controls::SwapChainPanel;
using Microsoft::UI::Xaml::Input::PointerRoutedEventArgs;
using Microsoft::UI::Xaml::Input::TappedRoutedEventArgs;
using Microsoft::UI::Xaml::Navigation::NavigationEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct TestPage1 : TestPage1T<TestPage1> {
  static constexpr int num_frames = 3;

private:
  winrt::com_ptr<ISwapChainPanelNative> bridge = nullptr;
  winrt::com_ptr<IDXGISwapChain3> swapchain = nullptr;

  winrt::com_ptr<ID3D11RenderTargetView> render_target = nullptr;
  winrt::com_ptr<ID3D11Texture2D> render_target_texture = nullptr;

  winrt::com_ptr<ID3D12Resource> d12_render_targets[num_frames]{};
  winrt::com_ptr<ID3D12CommandAllocator> d12_command_allocators[num_frames]{};
  winrt::com_ptr<ID3D12Fence> d12_fence = nullptr;
  HANDLE d12_fence_event = INVALID_HANDLE_VALUE;
  uint64_t d12_fence_values[num_frames]{};
  UINT frame_index = 0;

  DXGIProvider* dxgi = nullptr;
  DeviceProvider* devices = nullptr;
  Microsoft::UI::Dispatching::DispatcherQueue foreground = nullptr;

  Windows::Foundation::IAsyncAction action0 = nullptr;

public:
  TestPage1();
  ~TestPage1();

  void SetSwapChainPanel(SwapChainPanel panel);
  void Clear();

  /// @brief the page is loaded
  /// @see https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.page.onnavigatedto
  void OnNavigatedTo(const NavigationEventArgs& e);
  /// @brief the page will be unloaded
  /// @see https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.page.onnavigatedfrom
  void OnNavigatedFrom(const NavigationEventArgs& e);

  Windows::Foundation::IAsyncAction StartUpdate();

  void setup_graphics() noexcept(false);
  void update_swapchain(Windows::Foundation::Size size) noexcept(false);
  void update_render_target(ID3D11Device* device);

  void wait_for_gpu() noexcept(false);
  void update_frame_index() noexcept(false);

  void on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const&);
  void on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&);

  fire_and_forget update_description(winrt::hstring message);
  fire_and_forget update_progress(float ratio = 0.0f);

  IAsyncAction validate_assets(IInspectable const&, TappedRoutedEventArgs const&);

  void Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e);
  void updateSwitch_Toggled(IInspectable const&, RoutedEventArgs const&);
  void SwapchainPanel1_PointerEntered(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerMoved(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerExited(IInspectable const&, PointerRoutedEventArgs const&);
};
} // namespace winrt::Exp1::implementation

namespace winrt::Exp1::factory_implementation {
struct TestPage1 : TestPage1T<TestPage1, implementation::TestPage1> {};
} // namespace winrt::Exp1::factory_implementation