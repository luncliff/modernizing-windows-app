#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Microsoft.Windows.System.Power.h>

namespace winrt::PowerCpp::implementation
{
using Microsoft::Windows::System::Power::PowerManager;

MainWindow::MainWindow()
{
    // Xaml objects should not call InitializeComponent during construction.
    // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
    MainWindowT::InitializeComponent();

    PowerManager::BatteryStatus();
    // "Battery status: {PowerManager.BatteryStatus}"
    PowerManager::DisplayStatus();
    // "Display status: {PowerManager.DisplayStatus}"
    PowerManager::EffectivePowerMode();
    // "Effective power mode: {PowerManager.EffectivePowerMode}"
    PowerManager::EnergySaverStatus();
    // "Energy saver status: {PowerManager.EnergySaverStatus}"
    PowerManager::PowerSourceKind();
    // "Power source kind: {PowerManager.PowerSourceKind}"
    PowerManager::PowerSupplyStatus();
    // "Power supply status: {PowerManager.PowerSupplyStatus}"
    PowerManager::RemainingChargePercent();
    // "Power remaining charge percent: {PowerManager.RemainingChargePercent}"
    PowerManager::SystemSuspendStatus();
    // "System suspended status: {PowerManager.SystemSuspendStatus}"
    PowerManager::UserPresenceStatus();
    // "User presence status: {PowerManager.UserPresenceStatus}"
}

} // namespace winrt::PowerCpp::implementation
