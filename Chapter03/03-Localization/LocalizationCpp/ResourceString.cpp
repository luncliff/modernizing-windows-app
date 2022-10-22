#include "pch.h"

#include "ResourceString.h"
#if __has_include("ResourceString.g.cpp")
#include "ResourceString.g.cpp"
#endif

namespace winrt::LocalizationCpp::implementation {

	winrt::hstring ResourceString::Name() { return name; }

	void ResourceString::Name(winrt::hstring value) { name = value; }

	IInspectable ResourceString::ProvideValue() {
		auto& rm = AppResourceManager::Instance();
		winrt::hstring txt = rm.GetString(name);
		return winrt::box_value(txt);
	}

	IInspectable ResourceString::ProvideValue(IXamlServiceProvider const&) {
		return this->ProvideValue();
	}

} // namespace winrt::LocalizationCpp::implementation
