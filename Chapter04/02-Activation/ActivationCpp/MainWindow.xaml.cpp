#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::ActivationCpp::implementation
{
MainWindow::MainWindow()
{
    // Xaml objects should not call InitializeComponent during construction.
    // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    MainWindowT::InitializeComponent();
}

void MainWindow::StackPanel_Loaded(IInspectable const &, RoutedEventArgs const &)
{
    txtFilePath();
}

IStorageItem MainWindow::FilePath() const noexcept
{
    return m_filepath;
}
void MainWindow::FilePath(IStorageItem value) noexcept
{
    m_filepath = value;
}

} // namespace winrt::ActivationCpp::implementation
