#pragma once

#include "MainWindow.g.h"

namespace winrt::PowerCpp::implementation {
	using Microsoft::UI::Xaml::RoutedEventArgs;
	using Windows::Foundation::IInspectable;

	using winrt::Microsoft::Windows::System::Power::PowerManager;
	using winrt::Microsoft::Windows::System::Power::BatteryStatus;
	using winrt::Microsoft::Windows::System::Power::DisplayStatus;
	using winrt::Microsoft::Windows::System::Power::EffectivePowerMode;
	using winrt::Microsoft::Windows::System::Power::EnergySaverStatus;
	using winrt::Microsoft::Windows::System::Power::PowerSourceKind;
	using winrt::Microsoft::Windows::System::Power::PowerSupplyStatus;
	using winrt::Microsoft::Windows::System::Power::SystemSuspendStatus;
	using winrt::Microsoft::Windows::System::Power::UserPresenceStatus;

	struct MainWindow : MainWindowT<MainWindow> {
		MainWindow();

		BatteryStatus GetBatteryStatus() const noexcept;
		DisplayStatus GetDisplayStatus() const noexcept;
		EffectivePowerMode GetEffectivePowerMode() const noexcept;
		EnergySaverStatus GetEnergySaverStatus() const noexcept;
		PowerSourceKind GetPowerSourceKind() const noexcept;
		PowerSupplyStatus GetPowerSupplyStatus() const noexcept;
		int32_t GetRemainingChargePercent() const noexcept;
		SystemSuspendStatus GetSystemSuspendStatus() const noexcept;
		UserPresenceStatus GetUserPresenceStatus() const noexcept;
	};
} // namespace winrt::PowerCpp::implementation

namespace winrt::PowerCpp::factory_implementation {
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow> {};
} // namespace winrt::PowerCpp::factory_implementation
