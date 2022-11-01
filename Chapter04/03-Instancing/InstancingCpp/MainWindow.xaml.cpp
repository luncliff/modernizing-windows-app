#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "winrt/Windows.UI.Xaml.h"

namespace winrt::InstancingCpp::implementation {
	using Microsoft::UI::Xaml::Application;
	using Microsoft::UI::Xaml::IApplication;

	MainWindow::MainWindow() {
		InitializeComponent();
		RegisterEvents();
	}

	void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&) {
		myButton().Content(box_value(L"Clicked"));
	}

	void MainWindow::RegisterEvents() {
		try {
			// note: we need Microsoft.UI.Xaml.Application
			//	this will always nullptr
			auto current = Application::Current().try_as<Windows::UI::Xaml::Application>();
			if (current == nullptr)
				return;
			on_suspend_token =
				current.Suspending({ get_weak(), &MainWindow::OnSuspend });
			on_resume_token =
				current.Resuming({ get_weak(), &MainWindow::OnResume });
		}
		catch (const winrt::hresult_error& ex) {
			spdlog::error("{}: {}", "MainWindow", winrt::to_string(ex.message()));
		}
	}
	void MainWindow::UnregisterEvents() {
		try {
			// note: we need Microsoft.UI.Xaml.Application
			//	this will always nullptr
			auto current = Application::Current().try_as<Windows::UI::Xaml::Application>();
			if (current == nullptr)
				return;
			current.Suspending(on_suspend_token);
			current.Resuming(on_resume_token);
		}
		catch (const winrt::hresult_error& ex) {
			spdlog::error("{}: {}", "MainWindow", winrt::to_string(ex.message()));
		}
	}

	void MainWindow::OnSuspend(IInspectable const&, SuspendingEventArgs const&) {
		spdlog::info("{}: {}", "MainWindow", __func__);
	}

	void MainWindow::OnResume(IInspectable const&, IInspectable const&) {
		spdlog::info("{}: {}", "MainWindow", __func__);
	}
} // namespace winrt::InstancingCpp::implementation
