#pragma once
#include <Windows.h>
#include <d3d12.h>

#undef GetCurrentTime
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Notifications.h>

// MIDL headers
#include <hstring.h>
#include <restrictederrorinfo.h>
#include <unknwn.h>

#define FMT_HEADER_ONLY
#define SPDLOG_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

namespace winrt::SwapchainPanelTest {

void set_log_stream(const char* name);

void GetAssetsPath(WCHAR* path, UINT pathSize);

HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size);

HRESULT ReadDataFromDDSFile(LPCWSTR filename, byte** data, UINT* offset,
                            UINT* size);

void SetName(ID3D12Object* pObject, LPCWSTR name);
void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index);
winrt::com_ptr<ID3DBlob> CompileShader(const std::wstring& filename,
                                       const D3D_SHADER_MACRO* defines,
                                       const std::string& entrypoint,
                                       const std::string& target);

} // namespace winrt::SwapchainPanelTest
