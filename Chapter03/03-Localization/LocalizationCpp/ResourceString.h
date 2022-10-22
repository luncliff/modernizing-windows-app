#pragma once
#include "ResourceString.g.h"

#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.h>

namespace winrt::LocalizationCpp::implementation {
	using Microsoft::UI::Xaml::IXamlServiceProvider;
	using Windows::Foundation::IInspectable;

	struct ResourceString : ResourceStringT<ResourceString> {
	private:
		winrt::hstring name{};

	public:
		ResourceString() noexcept = default;

		winrt::hstring Name();
		void Name(winrt::hstring value);

		/// @see
		/// https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/boxing
		IInspectable ProvideValue();
		IInspectable ProvideValue(IXamlServiceProvider const&);
	};

} // namespace winrt::LocalizationCpp::implementation

namespace winrt::LocalizationCpp::factory_implementation {

	struct ResourceString
		: ResourceStringT<ResourceString, implementation::ResourceString> {};

} // namespace winrt::LocalizationCpp::factory_implementation

