#pragma once

#include "App.xaml.g.h"

namespace winrt::InstancingCpp::implementation {
	using Microsoft::UI::Xaml::LaunchActivatedEventArgs;
	using Microsoft::UI::Xaml::Window;
	using Windows::ApplicationModel::Activation::ActivationKind;
	using Windows::ApplicationModel::Activation::ApplicationExecutionState;
	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::IInspectable;

	/// @see Microsoft.UI.Xaml.2.h
	struct App : AppT<App> {
		App();

		void OnLaunched(LaunchActivatedEventArgs const&);

	private:
		Window window = nullptr;
	};
} // namespace winrt::InstancingCpp::implementation
