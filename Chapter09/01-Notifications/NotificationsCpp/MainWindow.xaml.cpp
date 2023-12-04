#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::NotificationsCpp::implementation
{
using namespace Microsoft::UI::Xaml;

MainWindow::MainWindow()
{
    // Xaml objects should not call InitializeComponent during construction.
    // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    MainWindowT::InitializeComponent();
}

void MainWindow::OnSendTextNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendImageNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendHeroImageNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendInlineHeroImageNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnCustomNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnTaggedNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnScheduledNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnDeleteScheduledNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendInteractiveNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendNotificationWithButtons(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendNotificationWithReply(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendNotificationWithDropdown(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendNotificationWithReminder(IInspectable const &s, RoutedEventArgs const &e)
{
}
IAsyncAction MainWindow::OnSendNotificationWithProgressBar(IInspectable const &s, RoutedEventArgs const &e)
{
    co_await std::suspend_never{};
}
void MainWindow::OnSendBadgeNumericNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}
void MainWindow::OnSendBadgeGlyphNotification(IInspectable const &s, RoutedEventArgs const &e)
{
}

} // namespace winrt::NotificationsCpp::implementation
