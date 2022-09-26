﻿#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace App1;
using namespace App1::implementation;

using winrt::Windows::System::DispatcherQueue;
using winrt::Windows::System::DispatcherQueueController;

void set_log_stream(const char* name);
void print_version_info();

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() {
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e) {
        if (IsDebuggerPresent()) {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
    set_log_stream("App");
    winrt::check_hresult(MFStartup(MF_VERSION, MFSTARTUP_FULL));
    print_version_info();
}

App::~App() {
    MFShutdown();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other
/// entry points will be used such as when the application is launched to open a
/// specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const&) {
    spdlog::info("{}", __func__);

    {
        const auto paths = winrt::Windows::Storage::AppDataPaths::GetDefault();
        spdlog::info("appdata:");
        spdlog::info(" {:s}: {:s}", "local", winrt::to_string(paths.LocalAppData()));
        spdlog::info(" {:s}: {:s}", "desktop", winrt::to_string(paths.Desktop()));
        spdlog::info(" {:s}: {:s}", "documents", winrt::to_string(paths.Documents()));
    }
    {
        const auto data = winrt::Windows::Storage::ApplicationData::Current();
        spdlog::info("storage:");
        spdlog::info(" {:s}: {:s}", "temp", winrt::to_string(data.TemporaryFolder().Path()));
        spdlog::info(" {:s}: {:s}", "local", winrt::to_string(data.LocalFolder().Path()));
        spdlog::info(" {:s}: {:s}", "local_cache", winrt::to_string(data.LocalCacheFolder().Path()));
        spdlog::info(" {:s}: {:s}", "shared_local", winrt::to_string(data.SharedLocalFolder().Path()));
    }
    controller = DispatcherQueueController::CreateOnDedicatedThread();
    queue = controller.DispatcherQueue();
    spdlog::debug("{}: {}", __func__, "DispatcherQueue are ready");
    window = make<MainWindow>();
    window.Activate();
}