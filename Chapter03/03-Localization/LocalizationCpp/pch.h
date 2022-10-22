#pragma once
#include <windows.h>

#undef GetCurrentTime
#include <wil/cppwinrt_helpers.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>

#include <hstring.h>
#include <restrictederrorinfo.h>
#include <unknwn.h>

namespace winrt::LocalizationCpp
{
	using Microsoft::Windows::ApplicationModel::Resources::ResourceContext;
	using Microsoft::Windows::ApplicationModel::Resources::ResourceManager;
	using Microsoft::Windows::ApplicationModel::Resources::ResourceMap;

	class AppResourceManager final {
		ResourceManager manager{};
		ResourceContext context = nullptr;
		ResourceMap main = nullptr;

	private:
		AppResourceManager() noexcept(false);

	public:
		static AppResourceManager& Instance();

		std::wstring MakeKey(winrt::hstring name);
		winrt::hstring GetString(winrt::hstring name);
		winrt::com_array<uint8_t> GetBytes(winrt::hstring name);
	};

} // namespace winrt::LocalizationCpp

#include "ResourceString.h"
