#include "pch.h"

#include <microsoft.ui.xaml.window.h>

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace std::experimental;

void send_notification(winrt::hstring title, winrt::hstring message);

winrt::Windows::Foundation::IAsyncAction open_video_file(HWND window);

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::App1::implementation {
MainWindow::MainWindow() {
    InitializeComponent();
    spdlog::info("{}", __func__);
    // see https://learn.microsoft.com/en-us/windows/apps/develop/ui-input/retrieve-hwnd
    auto native = this->try_as<::IWindowNative>();
    winrt::check_bool(native);
    if (winrt::hresult hr = native->get_WindowHandle(&hwnd); FAILED(hr))
        winrt::throw_hresult(hr);
    spdlog::info("{}: {}", "MainWindow", static_cast<void*>(hwnd));
}

int32_t MainWindow::MyProperty() {
    throw winrt::hresult_not_implemented();
}

void MainWindow::MyProperty(int32_t /* value */) {
    throw winrt::hresult_not_implemented();
}

Windows::Foundation::IAsyncAction MainWindow::DoAsync() {
    co_await winrt::resume_background();
    spdlog::debug("{}: {}", "MainWindow", __func__);
}

void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&) {
    send_notification(L"MainWindow", L"myButton_Click");
    myButton().Content(box_value(L"Clicked"));
}

void MainWindow::open_video_file_Click(IInspectable const&, RoutedEventArgs const&) {
    spdlog::debug("{}: {}", "MainWindow", __func__);
    /// @todo support winrt::Windows::UI::Core::CoreWindow
    open_video_file(hwnd);
}

Windows::Foundation::IAsyncAction MainWindow::change_page_Click(Windows::Foundation::IInspectable const& sender,
                                                                Microsoft::UI::Xaml::RoutedEventArgs const& args) {
    spdlog::debug("{}: {}", "MainWindow", __func__);
    co_await DoAsync();
    MediaSinkPage page{};
    this->Content(page);
    // ...
}

} // namespace winrt::App1::implementation
