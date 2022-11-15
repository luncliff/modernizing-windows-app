#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "TestPage1.xaml.h"

#include <microsoft.ui.xaml.window.h>

namespace winrt::SwapchainPanelTest::implementation {
using Windows::UI::Xaml::Interop::TypeKind;
using Windows::UI::Xaml::Interop::TypeName;

MainWindow::MainWindow() {
  InitializeComponent();
  // see
  // https://learn.microsoft.com/en-us/windows/apps/develop/ui-input/retrieve-hwnd
  auto native = this->try_as<::IWindowNative>();
  if (auto hr = native->get_WindowHandle(&hwnd); FAILED(hr))
    winrt::throw_hresult(hr);
  spdlog::info("{}: HWND {:p}", "MainWindow", static_cast<void*>(hwnd));
  // create/update pages
  page1 = winrt::make<implementation::TestPage1>();
  ShellFrame().Content(page1);
}

Page MainWindow::GetPage1() {
  return page1;
}

void MainWindow::use(DeviceResources* ptr) noexcept {
  if (ptr == nullptr)
    throw winrt::hresult_invalid_argument{};

  spdlog::info("{}: {}", "MainWindow", "updating GPUResources");
  device_resources = ptr;
  if (auto p = page1.as<implementation::TestPage1>(); p) {
    p->use(device_resources);
  }
}

void MainWindow::on_window_size_changed(IInspectable const&,
                                        WindowSizeChangedEventArgs const& e) {
  auto s = e.Size();
  spdlog::info("{}: size ({:.2f},{:.2f})", "MainWindow", s.Width, s.Height);
}

void MainWindow::on_window_visibility_changed(
    IInspectable const&, WindowVisibilityChangedEventArgs const& e) {
  spdlog::info("{}: visibility {}", "MainWindow", e.Visible());
}

void MainWindow::on_item_invoked(NavigationView const&,
                                 NavigationViewItemInvokedEventArgs const&) {
  spdlog::info("{}: {}", "MainWindow", __func__);
  Frame frame = ShellFrame();
  // ...
}

void MainWindow::on_back_requested(
    NavigationView const&, NavigationViewBackRequestedEventArgs const&) {
  spdlog::info("{}: {}", "MainWindow", __func__);
  Frame frame = ShellFrame();
  if (frame.CanGoBack())
    frame.GoBack();
}
} // namespace winrt::SwapchainPanelTest::implementation
