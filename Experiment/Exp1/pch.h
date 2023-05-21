#pragma once
#include <Windows.h>

#include <d3d11_4.h>
#include <d3d12.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>

// WindowsAppSDK headers
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
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
#include <winrt/Microsoft.Windows.ApplicationModel.WindowsAppRuntime.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Notifications.h>

// MIDL headers
#include <hstring.h>
#include <restrictederrorinfo.h>
#include <unknwn.h>

// Standard C++
#include <filesystem>

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_WCHAR_FILENAMES
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>

#include "winrt_fmt_helper.hpp"

namespace winrt::Exp1 {

void set_log_stream(const char* name);

/**
 * @see https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew
 * @see GetModuleFileNameW
 */
DWORD get_module_path(WCHAR* path, UINT capacity) noexcept(false);
std::filesystem::path get_module_path() noexcept(false);

void SetNameIndexed(ID3D12Object* object, LPCWSTR name, UINT index);

Windows::Foundation::IAsyncAction read_cso(Windows::Storage::StorageFile file,
                                           Windows::Storage::Streams::Buffer& output) noexcept(false);
Windows::Foundation::IAsyncAction make_shader_from_cso(Windows::Storage::StorageFile file, ID3D11Device* device,
                                                       ID3D11VertexShader** output) noexcept(false);
Windows::Foundation::IAsyncAction make_shader_from_cso(Windows::Storage::StorageFile file, ID3D11Device* device,
                                                       ID3D11PixelShader** output) noexcept(false);

//winrt::com_ptr<ID3DBlob> make_compiler_bytecode(Windows::Storage::StorageFile, const char* entrypoint,
//                                                const char* target);

} // namespace winrt::Exp1
