#include "pch.h"

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <winrt/base.h>

#include "App.xaml.h"

namespace winrt::InstancingCpp {
	// ...

	bool has_env(const char* key) noexcept {
		size_t len = 0;
		char buf[40]{};
		if (auto ec = getenv_s(&len, buf, key); ec != 0)
			return false;
		std::string_view value{ buf, len };
		return value.empty() == false;
	}

	auto make_logger(const char* name, FILE* fout) noexcept(false) {
		using namespace spdlog::sinks;
		std::vector<spdlog::sink_ptr> sinks{};
#if defined(_DEBUG)
		sinks.emplace_back(std::make_shared<msvc_sink_st>());
#endif
		if (fout == stdout)
			sinks.emplace_back(std::make_shared<wincolor_stdout_sink_st>());
		else if (fout == stderr)
			sinks.emplace_back(std::make_shared<wincolor_stderr_sink_st>());
		else {
			using mutex_t = spdlog::details::console_nullmutex;
			using sink_t = spdlog::sinks::stdout_sink_base<mutex_t>;
			sinks.emplace_back(std::make_shared<sink_t>(fout));
		}
		return std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
	}

	void set_log_stream(const char* name) {
		std::shared_ptr logger = make_logger(name, stdout);
		logger->set_pattern("%T.%e [%L] %8t %v");
#if defined(_DEBUG)
		logger->set_level(spdlog::level::level_enum::debug);
#endif
		if (has_env("CI"))
			logger->set_level(spdlog::level::level_enum::trace);
		spdlog::set_default_logger(logger);
	}

} // namespace winrt::InstancingCpp

using namespace winrt::InstancingCpp;

// see
// https://learn.microsoft.com/en-us/windows/windows-app-sdk/api/winrt/microsoft.ui.xaml.application
using winrt::Microsoft::UI::Xaml::Application;
using winrt::Microsoft::UI::Xaml::ApplicationInitializationCallback;
using winrt::Microsoft::UI::Xaml::ApplicationInitializationCallbackParams;
using winrt::Microsoft::UI::Xaml::IApplicationOverrides;
using winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments;
using winrt::Microsoft::Windows::AppLifecycle::AppInstance;
using winrt::Microsoft::Windows::AppLifecycle::ExtendedActivationKind;
using winrt::Windows::ApplicationModel::Activation::IFileActivatedEventArgs;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Storage::IStorageItem;

IAsyncAction decide_redirection(bool& redirected) {
	redirected = false;
	auto instance = AppInstance::GetCurrent();
	AppActivationArguments activation = instance.GetActivatedEventArgs();
	if (activation.Kind() == ExtendedActivationKind::File) {
		if (auto args = activation.Data().try_as<IFileActivatedEventArgs>();
			args != nullptr) {
			IStorageItem file = args.Files().GetAt(0);
			instance = AppInstance::FindOrRegisterForKey(file.Name());
			if ((instance != nullptr) && (instance.IsCurrent() == false)) {
				redirected = true;
				co_await instance.RedirectActivationToAsync(activation);
			}
		}
	}
	co_return;
}

IAsyncAction async_main(ApplicationInitializationCallbackParams const&) {
	spdlog::info("{}: {}", "async_main", __func__);

	bool redirected = false;
	co_await decide_redirection(redirected);

	Application app = winrt::make<implementation::App>();
	spdlog::info("{}: {}", "async_main", "created application");
}

std::vector<std::wstring_view> arguments{};

void setup_arguments(HINSTANCE, PWSTR cmdline, int) {
	int argc = 0;
	wchar_t** argv = CommandLineToArgvW(cmdline, &argc);
	if (argc)
		spdlog::info("arguments:");
	for (int i = 0; i < argc; ++i) {
		auto len = lstrlenW(argv[i]);
		arguments.emplace_back(argv[i], len);
		spdlog::info(" - {}", winrt::to_string(arguments[i]));
	}
}

/// @see https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR cmdline, int cmdshow) {
	winrt::init_apartment(); // use winrt::apartment_type::single_threaded?
	set_log_stream("main");
	spdlog::info("{}: {:p}", "instance", reinterpret_cast<void*>(instance));

	setup_arguments(instance, cmdline, cmdshow);

	Application::Start(async_main);
	return EXIT_SUCCESS;
}
