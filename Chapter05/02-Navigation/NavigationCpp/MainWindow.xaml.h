#pragma once
#include "MainWindow.g.h"

namespace winrt::NavigationCpp::implementation
{
	using Microsoft::UI::Xaml::Window;
	using Microsoft::UI::Xaml::Controls::Frame;
	using Microsoft::UI::Xaml::Controls::Page;
	using Microsoft::UI::Xaml::Controls::NavigationView;
	using Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs;
	using Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs;

	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

		void NavigationView_ItemInvoked(NavigationView const& sender, NavigationViewItemInvokedEventArgs const& args);
		void NavigationView_BackRequested(NavigationView const& sender, NavigationViewBackRequestedEventArgs const& args);
	private:
		Page homepage = nullptr;
	};
}

namespace winrt::NavigationCpp::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
