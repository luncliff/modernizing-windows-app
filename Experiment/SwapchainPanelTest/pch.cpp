#include "pch.h"

#include <cstdio>
#include <fmt/chrono.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <D3Dcompiler.h>

namespace winrt::SwapchainPanelTest {

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


std::wstring mb2w(std::string_view in) noexcept(false) {
  std::wstring out{};
  out.reserve(in.length());
  const char* ptr = in.data();
  const char* const end = in.data() + in.length();
  mbstate_t state{};
  wchar_t wc{};
  while (size_t len = mbrtowc(&wc, ptr, end - ptr, &state)) {
    if (len == static_cast<size_t>(-1)) // bad encoding
      throw std::system_error{errno, std::system_category(), "mbrtowc"};
    if (len == static_cast<size_t>(-2)) // valid but incomplete
      break;                            // nothing to do more
    out.push_back(wc);
    ptr += len; // advance [1...n]
  }
  return out;
}

UINT CalculateConstantBufferByteSize(UINT byteSize) {
  // Constant buffer size is required to be aligned.
  return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) &
         ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

void GetAssetsPath(WCHAR* path, UINT pathSize) {
  if (path == nullptr) {
    throw std::exception();
  }

  DWORD size = GetModuleFileName(nullptr, path, pathSize);
  if (size == 0 || size == pathSize) {
    // Method failed or path was truncated.
    throw std::exception();
  }

  WCHAR* lastSlash = wcsrchr(path, L'\\');
  if (lastSlash) {
    *(lastSlash + 1) = L'\0';
  }
}

HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size) {
#if WINVER >= _WIN32_WINNT_WIN8
  CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
  extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
  extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
  extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
  extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
  extendedParams.lpSecurityAttributes = nullptr;
  extendedParams.hTemplateFile = nullptr;

  HANDLE file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ,
                          OPEN_EXISTING, &extendedParams));
#else
  HANDLE file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
                             SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                         nullptr));
#endif
  if (file == INVALID_HANDLE_VALUE) {
    throw std::exception();
  }

  FILE_STANDARD_INFO fileInfo = {};
  if (!GetFileInformationByHandleEx(file, FileStandardInfo, &fileInfo,
                                    sizeof(fileInfo))) {
    throw std::exception();
  }

  if (fileInfo.EndOfFile.HighPart != 0) {
    throw std::exception();
  }

  *data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
  *size = fileInfo.EndOfFile.LowPart;

  if (!ReadFile(file, *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr)) {
    throw std::exception();
  }

  CloseHandle(file);
  return S_OK;
}

HRESULT ReadDataFromDDSFile(LPCWSTR filename, byte** data, UINT* offset,
                            UINT* size) {
  if (FAILED(ReadDataFromFile(filename, data, size))) {
    return E_FAIL;
  }

  // DDS files always start with the same magic number.
  static const UINT DDS_MAGIC = 0x20534444;
  UINT magicNumber = *reinterpret_cast<const UINT*>(*data);
  if (magicNumber != DDS_MAGIC) {
    return E_FAIL;
  }

  struct DDS_PIXELFORMAT {
    UINT size;
    UINT flags;
    UINT fourCC;
    UINT rgbBitCount;
    UINT rBitMask;
    UINT gBitMask;
    UINT bBitMask;
    UINT aBitMask;
  };

  struct DDS_HEADER {
    UINT size;
    UINT flags;
    UINT height;
    UINT width;
    UINT pitchOrLinearSize;
    UINT depth;
    UINT mipMapCount;
    UINT reserved1[11];
    DDS_PIXELFORMAT ddsPixelFormat;
    UINT caps;
    UINT caps2;
    UINT caps3;
    UINT caps4;
    UINT reserved2;
  };

  auto ddsHeader = reinterpret_cast<const DDS_HEADER*>(*data + sizeof(UINT));
  if (ddsHeader->size != sizeof(DDS_HEADER) ||
      ddsHeader->ddsPixelFormat.size != sizeof(DDS_PIXELFORMAT)) {
    return E_FAIL;
  }

  const ptrdiff_t ddsDataOffset = sizeof(UINT) + sizeof(DDS_HEADER);
  *offset = ddsDataOffset;
  *size = *size - ddsDataOffset;

  return S_OK;
}

void SetName(ID3D12Object* pObject, LPCWSTR name) {
  pObject->SetName(name);
}
void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index) {
  WCHAR fullName[50];
  if (swprintf_s(fullName, L"%s[%u]", name, index) > 0) {
    pObject->SetName(fullName);
  }
}
winrt::com_ptr<ID3DBlob> CompileShader(const std::wstring& filename,
                                       const D3D_SHADER_MACRO* defines,
                                       const std::string& entrypoint,
                                       const std::string& target) {
  UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
  compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  HRESULT hr;

  winrt::com_ptr<ID3DBlob> byteCode = nullptr;
  winrt::com_ptr<ID3DBlob> errors = nullptr;
  hr = D3DCompileFromFile(filename.c_str(), defines,
                          D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(),
                          target.c_str(), compileFlags, 0, byteCode.put(),
                          errors.put());

  if (errors != nullptr) {
    OutputDebugStringA((char*)errors->GetBufferPointer());
  }
  winrt::check_hresult(hr);

  return byteCode;
}

} // namespace winrt::SwapchainPanelTest
