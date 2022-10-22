#pragma once

#include "MainWindow.g.h"

namespace winrt::DispatcherCpp::implementation
{
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IInspectable;
	using Microsoft::UI::Xaml::RoutedEventArgs;

	struct MainWindow : MainWindowT<MainWindow>
	{
	private:
		Microsoft::UI::Dispatching::DispatcherQueue dispatcher = nullptr;

	public:
		MainWindow();

		IAsyncAction OnUpdateMessage(IInspectable const& sender, RoutedEventArgs const& args);
	};
}

namespace winrt::DispatcherCpp::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
