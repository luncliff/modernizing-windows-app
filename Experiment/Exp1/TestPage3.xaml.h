#pragma once
#include "TestPage3.g.h"

#include "DeviceProvider.h"
#include <d3d9.h>
#include <dxva2api.h>
#include <evr.h>

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

struct TestPage3 : TestPage3T<TestPage3> {
private:
  winrt::com_ptr<ISwapChainPanelNative> bridge = nullptr;
  winrt::com_ptr<IDXGISwapChain3> swapchain = nullptr;

  winrt::com_ptr<ID3D11RenderTargetView> render_target = nullptr;
  winrt::com_ptr<ID3D11Texture2D> render_target_texture = nullptr;
  DXGIProvider* dxgi = nullptr;
  DeviceProvider* devices = nullptr;

  UINT dxgi_manager_token = 0;
  winrt::com_ptr<IMFDXGIDeviceManager> dxgi_manager = nullptr;
  HANDLE dxgi_handle = nullptr;
  winrt::com_ptr<ID3D11VideoDevice> video_device = nullptr;

public:
  TestPage3();
  ~TestPage3();

  void SetSwapChainPanel(SwapChainPanel panel);

  /// @brief the page is loaded
  /// @see https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.page.onnavigatedto
  void OnNavigatedTo(const Microsoft::UI::Xaml::Navigation::NavigationEventArgs& e);
  /// @brief the page will be unloaded
  /// @see https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.page.onnavigatedfrom
  void OnNavigatedFrom(const Microsoft::UI::Xaml::Navigation::NavigationEventArgs& e);

  void update_swapchain(Windows::Foundation::Size size) noexcept(false);
  void update_render_target(ID3D11Device* device);

  void on_panel_size_changed(IInspectable const&, SizeChangedEventArgs const&);
  void on_panel_tapped(IInspectable const&, TappedRoutedEventArgs const&);

  void Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e);
  void Page_Unloaded(winrt::Windows::Foundation::IInspectable const& sender,
                     winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

  void SwapchainPanel1_PointerEntered(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerMoved(IInspectable const&, PointerRoutedEventArgs const&);
  void SwapchainPanel1_PointerExited(IInspectable const&, PointerRoutedEventArgs const&);
};
} // namespace winrt::Exp1::implementation

namespace winrt::Exp1::factory_implementation {
struct TestPage3 : TestPage3T<TestPage3, implementation::TestPage3> {};
} // namespace winrt::Exp1::factory_implementation
