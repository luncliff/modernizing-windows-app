//#define INITGUID
#define FMT_HEADER_ONLY
#define SPDLOG_HEADER_ONLY
#include "pch.h"

#include <D3Dcompiler.h>
#include <cstdio>
#include <d3d9.h>
#include <mmdeviceapi.h>

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace winrt::Exp1 {

auto make_logger(const char* name, FILE* fout) noexcept(false) {
  using namespace spdlog::sinks;
  std::vector<spdlog::sink_ptr> sinks{};
#if defined(_DEBUG)
  sinks.emplace_back(std::make_shared<msvc_sink_st>());
#endif
  if (fout == stdout)
    sinks.emplace_back(std::make_shared<wincolor_stdout_sink_st>());
  else if (fout == stderr)
    sinks.emplace_back(std::make_shared<wincolor_stderr_sink_st>());
  else {
    using mutex_t = spdlog::details::console_nullmutex;
    using sink_t = spdlog::sinks::stdout_sink_base<mutex_t>;
    sinks.emplace_back(std::make_shared<sink_t>(fout));
  }
  return std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
}

void set_log_stream(const char* name) {
  std::shared_ptr logger = make_logger(name, stdout);
  logger->set_pattern("%T.%e [%L] %8t %v");
#if defined(_DEBUG)
  logger->set_level(spdlog::level::level_enum::debug);
#endif
  spdlog::set_default_logger(logger);
}

UINT CalculateConstantBufferByteSize(UINT byteSize) {
  // Constant buffer size is required to be aligned.
  return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) &
         ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

DWORD get_module_path(WCHAR* path, UINT capacity) noexcept(false) {
  if (path == nullptr)
    throw std::invalid_argument{__func__};
  DWORD size = GetModuleFileNameW(nullptr, path, capacity);
  if (size == 0)
    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "GetModuleFileNameW"};
  return size;
}

std::filesystem::path get_module_path() noexcept(false) {
  wchar_t buf[260]{};
  auto len = get_module_path(buf, 260);
  //std::locale loc{".65001"};
  //return std::filesystem::path{std::wstring_view{buf, len}, loc};
  return std::wstring_view{buf, len};
}

void SetNameIndexed(ID3D12Object* object, LPCWSTR name, UINT index) {
  WCHAR buf[50]{};
  if (swprintf_s(buf, L"%s[%u]", name, index) > 0)
    object->SetName(buf);
}

winrt::com_ptr<ID3DBlob> make_compiler_bytecode(std::filesystem::path p, const char* entrypoint, const char* target) {
  spdlog::info("{}: shader {} entry {} file {}", __func__, target, entrypoint, p);
  const D3D_SHADER_MACRO* defines = nullptr;
  winrt::com_ptr<ID3DBlob> bytecode = nullptr;
  winrt::com_ptr<ID3DBlob> errors = nullptr;
  auto fpath = p.generic_wstring();
  HRESULT hr = D3DCompileFromFile(fpath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint, target,
                                  D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, bytecode.put(), errors.put());
  if (errors != nullptr) {
    std::string_view msg{reinterpret_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize()};
    spdlog::error("{}: {}", __func__, msg);
  }
  winrt::check_hresult(hr);
  return bytecode;
}

Windows::Foundation::IAsyncAction read_cso(Windows::Storage::StorageFile file,
                                           Windows::Storage::Streams::Buffer& output) noexcept(false) {
  Windows::Storage::Streams::IRandomAccessStream stream = co_await file.OpenReadAsync();
  auto capacity = static_cast<uint32_t>(stream.Size());
  Windows::Storage::Streams::Buffer buf(capacity);
  co_await stream.ReadAsync(buf, capacity, Windows::Storage::Streams::InputStreamOptions::None);
  output = buf;
}

Windows::Foundation::IAsyncAction make_shader_from_cso(Windows::Storage::StorageFile file, ID3D11Device* device,
                                                       ID3D11VertexShader** output) noexcept(false) {
  Windows::Storage::Streams::Buffer buf = nullptr;
  co_await read_cso(file, buf);
  if (auto hr = device->CreateVertexShader(buf.data(), buf.Length(), nullptr, output); FAILED(hr)) {
    spdlog::error("{}: {} file {}", __func__, "CreateVertexShader", file.Path());
    winrt::throw_hresult(hr);
  }
}

Windows::Foundation::IAsyncAction make_shader_from_cso(Windows::Storage::StorageFile file, ID3D11Device* device,
                                                       ID3D11PixelShader** output) noexcept(false) {
  Windows::Storage::Streams::Buffer buf = nullptr;
  co_await read_cso(file, buf);
  if (auto hr = device->CreatePixelShader(buf.data(), buf.Length(), nullptr, output); FAILED(hr)) {
    spdlog::error("{}: {} file {}", __func__, "CreatePixelShader", file.Path());
    winrt::throw_hresult(hr);
  }
}

} // namespace winrt::Exp1
