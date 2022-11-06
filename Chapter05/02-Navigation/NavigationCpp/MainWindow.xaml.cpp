#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "HomePage.xaml.h"

namespace winrt::NavigationCpp::implementation
{
	using Windows::UI::Xaml::Interop::TypeName;
	using Windows::UI::Xaml::Interop::TypeKind;

	MainWindow::MainWindow()
	{
		InitializeComponent();
		homepage = winrt::make<implementation::HomePage>();
		ShellFrame().Content(homepage);
	}

	void MainWindow::NavigationView_ItemInvoked(NavigationView const& sender, NavigationViewItemInvokedEventArgs const& args) {
		Frame frame = ShellFrame();
		if (true) {
			// see XamlTypeInfo.g.cpp
			TypeName name{ L"NavigationCpp.HomePage", TypeKind::Custom };
			frame.Navigate(name);
		}
	}

	void MainWindow::NavigationView_BackRequested(NavigationView const& sender, NavigationViewBackRequestedEventArgs const& args) {
		Frame frame = ShellFrame();
		if (frame.CanGoBack())
			frame.GoBack();
	}

}
