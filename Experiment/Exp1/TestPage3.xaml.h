#pragma once
#include "TestPage3.g.h"

#include "DeviceProvider.h"
#include "NV12TextureRenderer.h"

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
using Windows::Foundation::Uri;
using Windows::Storage::StorageFile;

struct TestPage3 : TestPage3T<TestPage3> {
private:
  winrt::com_ptr<ISwapChainPanelNative> bridge = nullptr;
  winrt::com_ptr<IDXGISwapChain3> swapchain = nullptr;

  winrt::com_ptr<ID3D11RenderTargetView> render_target = nullptr;
  winrt::com_ptr<ID3D11Texture2D> render_target_texture = nullptr;
  DXGIProvider* dxgi = nullptr;
  DeviceProvider* devices = nullptr;

  std::unique_ptr<NV12TextureRenderer> renderer = nullptr;
  Windows::Foundation::IAsyncAction action_update = nullptr;

private:
  void update_swapchain(Windows::Foundation::Size size) noexcept(false);
  void update_render_target(ID3D11Device* device);

  static void on_step_timer_update(TestPage3*) noexcept;

public:
  TestPage3();
  ~TestPage3();

  void SetSwapChainPanel(SwapChainPanel panel);
  Windows::Foundation::IAsyncAction RunUpdate();
  void CancelUpdate();

  void OnNavigatedTo(const NavigationEventArgs&);
  void OnNavigatedFrom(const NavigationEventArgs&);

  void on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const&);
  void on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&);

  void Page_Loaded(IInspectable const&, RoutedEventArgs const&);
  void Page_Unloaded(IInspectable const&, RoutedEventArgs const&);
  void ToggleSwitch0_Toggled(IInspectable const&, RoutedEventArgs const&);
  void SwapchainPanel1_PointerEntered(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerMoved(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerExited(IInspectable const&, PointerRoutedEventArgs const&);
};
} // namespace winrt::Exp1::implementation

namespace winrt::Exp1::factory_implementation {
struct TestPage3 : TestPage3T<TestPage3, implementation::TestPage3> {};
} // namespace winrt::Exp1::factory_implementation
