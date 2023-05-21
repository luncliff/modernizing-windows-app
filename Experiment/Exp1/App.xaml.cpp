#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::Exp1::implementation {
using namespace std::placeholders;

App::App() {
  InitializeComponent();
  set_log_stream("App");
  UnhandledException(std::bind(&App::on_unhandled_exception, this, _1, _2));
  spdlog::info("{}", get_module_path());
  // setup, version info...
  MFStartup(MF_VERSION, MFSTARTUP_FULL);
  spdlog::info("C++/WinRT:");
  spdlog::info("  version: {:s}", CPPWINRT_VERSION);
  //Windows::Storage::KnownFolders::DocumentsLibrary();
}

void App::on_unhandled_exception(IInspectable const&, UnhandledExceptionEventArgs const& e) {
  auto message = e.Message();
  spdlog::critical("{}: {}", "App", message);
#if defined(_DEBUG) && !defined(DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION)
  if (IsDebuggerPresent()) {
    __debugbreak();
  }
#endif
}

void App::OnLaunched(LaunchActivatedEventArgs const&) {
  spdlog::info("{}: {}", "App", __func__);
  resource_context = resource_manager.CreateResourceContext();
  resource_map = resource_manager.MainResourceMap();
  window = make<implementation::MainWindow>();
  if (auto w = window.as<implementation::MainWindow>(); w) {
    w->use(&dxgi, &devices);
  }
  window.Activate();
}

} // namespace winrt::Exp1::implementation
