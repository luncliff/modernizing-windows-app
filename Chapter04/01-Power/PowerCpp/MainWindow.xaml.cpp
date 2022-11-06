#include "pch.h"

#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

namespace winrt::PowerCpp::implementation {
	using winrt::Microsoft::Windows::System::Power::PowerManager;

	MainWindow::MainWindow() {
		InitializeComponent();

		auto make_hstring = [](std::wstring_view label, auto value) -> winrt::hstring {
			return winrt::hstring{ fmt::format(L"{}: {}", label, static_cast<int>(value)) };
		};

		txtBatteryStatus().Text(make_hstring(L"BatteryStatus", GetBatteryStatus()));
		txtDisplayStatus().Text(make_hstring(L"DisplayStatus", GetDisplayStatus()));
		txtEffectivePowerMode().Text(make_hstring(L"EffectivePowerMode", GetBatteryStatus()));
		txtEnergySaverStatus().Text(make_hstring(L"EnergySaverStatus", GetEnergySaverStatus()));
		txtPowerSourceKind().Text(make_hstring(L"PowerSourceKind", GetPowerSourceKind()));
		txtPowerSupplyStatus().Text(make_hstring(L"PowerSupplyStatus", GetPowerSupplyStatus()));
		//txtSystemSupendedStatus().Text(make_hstring(L"SystemSuspendStatus", GetSystemSuspendStatus()));
		txtUserPresenceStatus().Text(make_hstring(L"UserPresenceStatus", GetUserPresenceStatus()));
	}

	BatteryStatus MainWindow::GetBatteryStatus() const noexcept {
		return PowerManager::BatteryStatus();
	}
	DisplayStatus MainWindow::GetDisplayStatus() const noexcept {
		return PowerManager::DisplayStatus();
	}
	EffectivePowerMode MainWindow::GetEffectivePowerMode() const noexcept {
		return PowerManager::EffectivePowerMode().get();
	}
	EnergySaverStatus MainWindow::GetEnergySaverStatus() const noexcept {
		return PowerManager::EnergySaverStatus();
	}
	PowerSourceKind MainWindow::GetPowerSourceKind() const noexcept {
		return PowerManager::PowerSourceKind();
	}
	PowerSupplyStatus MainWindow::GetPowerSupplyStatus() const noexcept {
		return PowerManager::PowerSupplyStatus();
	}
	int32_t MainWindow::GetRemainingChargePercent() const noexcept {
		return PowerManager::RemainingChargePercent();
	}
	SystemSuspendStatus MainWindow::GetSystemSuspendStatus() const noexcept {
		return PowerManager::SystemSuspendStatus();
	}
	UserPresenceStatus MainWindow::GetUserPresenceStatus() const noexcept {
		return PowerManager::UserPresenceStatus();
	}

} // namespace winrt::PowerCpp::implementation
