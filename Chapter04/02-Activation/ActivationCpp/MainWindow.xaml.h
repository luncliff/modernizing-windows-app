#pragma once

#include "MainWindow.g.h"

#include <winrt/Windows.Storage.h>

namespace winrt::ActivationCpp::implementation
{
using Microsoft::UI::Xaml::RoutedEventArgs;
using Windows::Foundation::IInspectable;
using Windows::Storage::IStorageItem;

struct MainWindow : MainWindowT<MainWindow>
{
    MainWindow();

    void StackPanel_Loaded(IInspectable const &, RoutedEventArgs const &);

    IStorageItem FilePath() const noexcept;
    void FilePath(IStorageItem) noexcept;

  private:
    IStorageItem m_filepath;
};
} // namespace winrt::ActivationCpp::implementation

namespace winrt::ActivationCpp::factory_implementation
{
struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
{
};
} // namespace winrt::ActivationCpp::factory_implementation
