#pragma once

#include "MainWindow.g.h"

namespace winrt::LocalizationCpp::implementation
{
	using Windows::Foundation::IInspectable;
	using Microsoft::UI::Xaml::RoutedEventArgs;

	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

		void OnGetTranslation(IInspectable const& sender, RoutedEventArgs const& args);
	};
}

namespace winrt::LocalizationCpp::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
