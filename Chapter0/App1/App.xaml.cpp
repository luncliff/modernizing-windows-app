#include "pch.h"

#include "App.xaml.h"
#include "CustomPage1.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::App1::implementation {

void set_log_stream(const char* name);

App::App() {
    InitializeComponent();
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const& s, UnhandledExceptionEventArgs const& e) {
        if (IsDebuggerPresent()) {
            auto message = e.Message();
            spdlog::error("{}: {}", "App", winrt::to_string(message));
            __debugbreak();
        }
        on_unhandled_exception(s, e);
    });
#else
    UnhandledException(std::bind(&App::on_unhandled_exception, this));
#endif
}

void App::on_unhandled_exception(IInspectable const&, UnhandledExceptionEventArgs const& e) {
    auto message = e.Message();
    spdlog::critical("{}: {}", "App", winrt::to_string(message));
}

void App::OnLaunched(LaunchActivatedEventArgs const&) {
    set_log_stream("test");

    // common resources
    controller = DispatcherQueueController::CreateOnDedicatedThread();
    queue = controller.DispatcherQueue();

    // UI resources
    page1 = make<CustomPage1>();
    window1 = make<MainWindow>();
    if (auto w = window1.as<MainWindow>(); w) {
        w->BackgroundQueue(queue);
        w->Content(page1);
    }
    window1.Activate();
}
} // namespace winrt::App1::implementation