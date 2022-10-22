#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::LocalizationCpp::implementation
{
	using namespace Microsoft::UI::Xaml;

	MainWindow::MainWindow()
	{
		InitializeComponent();
	}

	void MainWindow::OnGetTranslation(IInspectable const&, RoutedEventArgs const&)
	{
		auto& rm = AppResourceManager::Instance();
		auto txt = rm.GetString(L"MyButton.Content");
		// ...
	}
}
