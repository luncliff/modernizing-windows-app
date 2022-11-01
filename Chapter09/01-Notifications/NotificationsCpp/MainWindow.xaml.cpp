#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::NotificationsCpp::implementation {
	using Microsoft::UI::Xaml::UIElement;
	using Microsoft::UI::Xaml::FrameworkElement;
	using Microsoft::UI::Xaml::ElementTheme;


	MainWindow::MainWindow() {
		InitializeComponent();
		FrameworkElement element = nullptr;
		if (this->Content().as(element); element == nullptr)
			throw winrt::hresult_not_implemented{};
		element.RequestedTheme(ElementTheme::Light);
	}

	void MainWindow::OnSendTextNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendImageNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendHeroImageNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendInlineHeroImageNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnCustomNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnTaggedNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnScheduledNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnDeleteScheduledNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendInteractiveNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendNotificationWithButtons(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendNotificationWithReply(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendNotificationWithDropdown(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendNotificationWithReminder(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	IAsyncAction
		MainWindow::OnSendNotificationWithProgressBar(IInspectable const&,
			RoutedEventArgs const&) {
		co_await std::experimental::suspend_never{};
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendBadgeNumericNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}
	void MainWindow::OnSendBadgeGlyphNotification(IInspectable const&,
		RoutedEventArgs const&) {
		spdlog::info("{:s}: {:s}", "MainWindow", __func__);
	}

} // namespace winrt::NotificationsCpp::implementation
