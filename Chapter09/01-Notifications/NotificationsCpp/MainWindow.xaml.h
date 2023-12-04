#pragma once

#include "MainWindow.g.h"

#include <winrt/Windows.UI.Notifications.h>

namespace winrt::NotificationsCpp::implementation
{
using Microsoft::UI::Xaml::RoutedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct MainWindow : MainWindowT<MainWindow>
{
    MainWindow();

    void OnSendTextNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendImageNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendHeroImageNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendInlineHeroImageNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnCustomNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnTaggedNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnScheduledNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnDeleteScheduledNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendInteractiveNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendNotificationWithButtons(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendNotificationWithReply(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendNotificationWithDropdown(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendNotificationWithReminder(IInspectable const &s, RoutedEventArgs const &e);
    IAsyncAction OnSendNotificationWithProgressBar(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendBadgeNumericNotification(IInspectable const &s, RoutedEventArgs const &e);
    void OnSendBadgeGlyphNotification(IInspectable const &s, RoutedEventArgs const &e);
};
} // namespace winrt::NotificationsCpp::implementation

namespace winrt::NotificationsCpp::factory_implementation
{
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
{
};
} // namespace winrt::NotificationsCpp::factory_implementation
