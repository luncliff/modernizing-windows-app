#pragma once

#include "MainWindow.g.h"

namespace winrt::NotificationsCpp::implementation {
using Microsoft::UI::Xaml::RoutedEventArgs;
using Windows::Foundation::IAsyncAction;
using Windows::Foundation::IInspectable;

struct MainWindow : MainWindowT<MainWindow> {
  MainWindow();

  void OnSendTextNotification(IInspectable const &sender,
                              RoutedEventArgs const &e);
  void OnSendImageNotification(IInspectable const &sender,
                               RoutedEventArgs const &e);
  void OnSendHeroImageNotification(IInspectable const &sender,
                                   RoutedEventArgs const &e);
  void OnSendInlineHeroImageNotification(IInspectable const &sender,
                                         RoutedEventArgs const &e);
  void OnCustomNotification(IInspectable const &sender,
                            RoutedEventArgs const &e);
  void OnTaggedNotification(IInspectable const &sender,
                            RoutedEventArgs const &e);
  void OnScheduledNotification(IInspectable const &sender,
                               RoutedEventArgs const &e);
  void OnDeleteScheduledNotification(IInspectable const &sender,
                                     RoutedEventArgs const &e);
  void OnSendInteractiveNotification(IInspectable const &sender,
                                     RoutedEventArgs const &e);
  void OnSendNotificationWithButtons(IInspectable const &sender,
                                     RoutedEventArgs const &e);
  void OnSendNotificationWithReply(IInspectable const &sender,
                                   RoutedEventArgs const &e);
  void OnSendNotificationWithDropdown(IInspectable const &sender,
                                      RoutedEventArgs const &e);
  void OnSendNotificationWithReminder(IInspectable const &sender,
                                      RoutedEventArgs const &e);
  IAsyncAction OnSendNotificationWithProgressBar(IInspectable const &sender,
                                                 RoutedEventArgs const &e);
  void OnSendBadgeNumericNotification(IInspectable const &sender,
                                      RoutedEventArgs const &e);
  void OnSendBadgeGlyphNotification(IInspectable const &sender,
                                    RoutedEventArgs const &e);
};
} // namespace winrt::NotificationsCpp::implementation

namespace winrt::NotificationsCpp::factory_implementation {
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::NotificationsCpp::factory_implementation
