#include "pch.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/xchar.h>

namespace winrt::LocalizationCpp {

	AppResourceManager::AppResourceManager() noexcept(false) {
		context = manager.CreateResourceContext();
		main = manager.MainResourceMap();
	}

	AppResourceManager& AppResourceManager::Instance() {
		static AppResourceManager instance{};
		return instance;
	}

	std::wstring AppResourceManager::MakeKey(winrt::hstring name) {
		auto key = fmt::format(L"Resources/{}", static_cast<std::wstring_view>(name));
		for (auto& c : key)
			if (c == L'.') c = L'/';
		return key;
	}

	winrt::hstring AppResourceManager::GetString(winrt::hstring name) {
		return main.GetValue(MakeKey(name)).ValueAsString();
	}

	winrt::com_array<uint8_t> AppResourceManager::GetBytes(winrt::hstring name) {
		return main.GetValue(MakeKey(name)).ValueAsBytes();
	}

}