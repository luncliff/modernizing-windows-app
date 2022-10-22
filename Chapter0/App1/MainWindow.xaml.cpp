#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::App1::implementation {

MainWindow::MainWindow() {
    InitializeComponent();
    // see https://learn.microsoft.com/en-us/windows/apps/develop/ui-input/retrieve-hwnd
    auto native = this->try_as<::IWindowNative>();
    if (auto hr = native->get_WindowHandle(&hwnd); FAILED(hr))
        winrt::throw_hresult(hr);
    spdlog::info("{}: HWND {:p}", "MainWindow", static_cast<void*>(hwnd));
}

Windows::System::DispatcherQueue MainWindow::BackgroundQueue() noexcept {
    return queue;
}

void MainWindow::BackgroundQueue(Windows::System::DispatcherQueue _queue) noexcept {
    queue = _queue;
}

void MainWindow::on_button1_clicked(IInspectable const&, RoutedEventArgs const&) {
    Button1().Content(box_value(L"Clicked"));
}

void MainWindow::on_button2_clicked(IInspectable const&, RoutedEventArgs const&) {
    Button2().Content(box_value(L"Clicked"));
}

void MainWindow::on_window_size_changed(IInspectable const&, WindowSizeChangedEventArgs const& e) {
    auto s = e.Size();
    spdlog::info("{}: size ({:.2f},{:.2f})", "MainWindow", s.Width, s.Height);
}

void MainWindow::on_window_visibility_changed(IInspectable const&, WindowVisibilityChangedEventArgs const& e) {
    spdlog::info("{}: visibility {}", "MainWindow", e.Visible());
}

IAsyncAction MainWindow::ChangeTheme() {
    co_await std::experimental::suspend_never{};
}

} // namespace winrt::App1::implementation
