#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::SwapchainPanelTest::implementation {
using namespace std::placeholders;

App::App() {
  InitializeComponent();
  // ...
  UnhandledException(std::bind(&App::on_unhandled_exception, this, _1, _2));
}

void App::on_unhandled_exception(IInspectable const&,
                                 UnhandledExceptionEventArgs const& e) {
  auto message = e.Message();
  spdlog::critical("{}: {}", "App", winrt::to_string(message));
#if defined _DEBUG &&                                                          \
    !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  if (IsDebuggerPresent()) {
    __debugbreak();
  }
#endif
}

void App::OnLaunched(LaunchActivatedEventArgs const&) {
  set_log_stream("App");

  spdlog::info("{}: {}", "App.OnLaunched", "creating resources");
  device_resources = std::make_unique<DeviceResources>();

  spdlog::info("{}: {}", "App.OnLaunched", "creating windows/pages");
  window = make<implementation::MainWindow>();
  if (auto w = window.as<implementation::MainWindow>(); w) {
    w->use(device_resources.get());
  }
  window.Activate();
}

} // namespace winrt::SwapchainPanelTest::implementation
