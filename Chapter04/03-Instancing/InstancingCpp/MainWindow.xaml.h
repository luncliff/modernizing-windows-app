#pragma once

#include "MainWindow.g.h"

namespace winrt::InstancingCpp::implementation {
	using Microsoft::UI::Xaml::RoutedEventArgs;
	using Windows::ApplicationModel::SuspendingEventArgs;
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IInspectable;

	struct MainWindow : MainWindowT<MainWindow> {
		MainWindow();

		void myButton_Click(IInspectable const& sender, RoutedEventArgs const& args);

		/// @see SuspendingEventHandler
		void OnSuspend(IInspectable const&, SuspendingEventArgs const&);
		/// @see Windows::Foundation::EventHandler<Windows::Foundation::IInspectable>
		void OnResume(IInspectable const&, IInspectable const&);

		void RegisterEvents();
		void UnregisterEvents();

	private:
		winrt::event_token on_suspend_token{};
		winrt::event_token on_resume_token{};
	};
} // namespace winrt::InstancingCpp::implementation

namespace winrt::InstancingCpp::factory_implementation {
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::InstancingCpp::factory_implementation
