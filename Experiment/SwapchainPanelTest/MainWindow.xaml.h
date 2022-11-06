#pragma once
#include "MainWindow.g.h"

#include "GPUResources.h"

namespace winrt::SwapchainPanelTest::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Microsoft::UI::Xaml::Window;
using Microsoft::UI::Xaml::WindowSizeChangedEventArgs;
using Microsoft::UI::Xaml::WindowVisibilityChangedEventArgs;
using Microsoft::UI::Xaml::Controls::Frame;
using Microsoft::UI::Xaml::Controls::NavigationView;
using Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs;
using Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs;
using Microsoft::UI::Xaml::Controls::Page;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct MainWindow : MainWindowT<MainWindow> {
private:
  HWND hwnd = nullptr;

public:
  MainWindow();

  Page GetPage1();

  void use(GPUResources*) noexcept;
  void on_window_size_changed(IInspectable const& sender,
                              WindowSizeChangedEventArgs const& e);
  void on_window_visibility_changed(IInspectable const& sender,
                                    WindowVisibilityChangedEventArgs const& e);
  void on_item_invoked(NavigationView const&,
                       NavigationViewItemInvokedEventArgs const&);
  void on_back_requested(NavigationView const&,
                         NavigationViewBackRequestedEventArgs const&);

private:
  GPUResources* gpu_resources = nullptr;
  Page page1 = nullptr;
};

} // namespace winrt::SwapchainPanelTest::implementation

namespace winrt::SwapchainPanelTest::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::SwapchainPanelTest::factory_implementation
