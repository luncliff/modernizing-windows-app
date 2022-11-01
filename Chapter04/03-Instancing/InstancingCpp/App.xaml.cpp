#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

namespace winrt::InstancingCpp::implementation {
	using namespace Microsoft::UI::Xaml;
	using namespace Microsoft::UI::Xaml::Controls;
	using namespace Microsoft::UI::Xaml::Navigation;
	using namespace Windows::ApplicationModel;
	using namespace Windows::ApplicationModel::Activation;

	App::App() {
		InitializeComponent();
#if defined _DEBUG &&                                                          \
    !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
		UnhandledException(
			[this](IInspectable const&, UnhandledExceptionEventArgs const& e) {
				if (IsDebuggerPresent()) {
					auto errorMessage = e.Message();
					__debugbreak();
				}
			});
#endif
	}

	void App::OnLaunched(LaunchActivatedEventArgs const& e) {
		if (e) {
			auto args = e.UWPLaunchActivatedEventArgs();
			switch (ApplicationExecutionState prev = args.PreviousExecutionState();
			prev) {
			default:
				spdlog::info("{}: {:X}", "ApplicationExecutionState",
					static_cast<uint32_t>(prev));
				break;
			}
			switch (ActivationKind kind = args.Kind(); kind) {
			default:
				spdlog::info("{}: {:X}", "ActivationKind", static_cast<uint32_t>(kind));
				break;
			}
		}
		window = make<MainWindow>();
		window.Activate();
	}

} // namespace winrt::InstancingCpp::implementation