#include "pch.h"

#include <DirectXTK/DirectXHelpers.h>
#include <dxva2api.h>
#include <evr.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include "NV12TextureRenderer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "evr.lib")

namespace winrt::Exp1 {

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT4X4;
using Windows::Foundation::IInspectable;
using Windows::Foundation::Uri;
using Windows::Storage::StorageFile;
using Windows::Storage::Streams::InputStreamOptions;

using namespace Windows::Foundation::Numerics;

inline float3 cast(const XMFLOAT3& v) {
  return *reinterpret_cast<const float3*>(&v);
};
inline XMFLOAT4X4 cast(const float4x4& m) {
  return *reinterpret_cast<const XMFLOAT4X4*>(&m);
};

struct VertexPositionTex {
  XMFLOAT3 pos;
  XMFLOAT2 tex;
};

struct QuadModelConstantBuffer {

  XMFLOAT4X4 model;
  XMFLOAT2 texCoordScale;
  XMFLOAT2 texCoordOffset;
};

NV12TextureRenderer::NV12TextureRenderer(ID3D11Device* _device, ID3D11DeviceContext* _context) noexcept(false) {
  if (_device == nullptr || _context == nullptr)
    winrt::hresult_invalid_argument{__FUNCTIONW__};
  _device->QueryInterface(device.put());
  _context->QueryInterface(context.put());
}

NV12TextureRenderer::~NV12TextureRenderer() noexcept {
  texture = nullptr;
  luma = nullptr;
  chroma = nullptr;
}

IAsyncAction NV12TextureRenderer::setup_resources() noexcept(false) {
  // "vs_5_0"
  if (vs == nullptr) {
    Uri uri{L"ms-appx:///QuadVertexShader.cso"};
    StorageFile vsfile = co_await StorageFile::GetFileFromApplicationUriAsync(uri);
    Windows::Storage::Streams::Buffer buf = nullptr;
    co_await read_cso(vsfile, buf);
    if (auto hr = device->CreateVertexShader(buf.data(), buf.Length(), nullptr, vs.put()); FAILED(hr)) {
      spdlog::error("{}: {} file {}", __func__, "CreateVertexShader", vsfile.Path());
      winrt::throw_hresult(hr);
    }
    // input layout
    std::array<D3D11_INPUT_ELEMENT_DESC, 2> elements{{
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionTex, pos), D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPositionTex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0},
    }};
    if (auto hr = device->CreateInputLayout(elements.data(), elements.size(), buf.data(), buf.Length(), layout.put());
        FAILED(hr)) {
      spdlog::error("{}: {} {}", __func__, "CreateInputLayout", winrt::hresult_error{hr});
      winrt::throw_hresult(hr);
    }
  }
  // "ps_5_0"
  if (ps == nullptr) {
    Uri uri{L"ms-appx:///QuadPixelShaderNV12.cso"};
    StorageFile psfile = co_await StorageFile::GetFileFromApplicationUriAsync(uri);
    Windows::Storage::Streams::Buffer buf = nullptr;
    co_await read_cso(psfile, buf);
    // create shader
    if (auto hr = device->CreatePixelShader(buf.data(), buf.Length(), nullptr, ps.put()); FAILED(hr)) {
      spdlog::error("{}: {} file {}", __func__, "CreatePixelShader", psfile.Path());
      winrt::throw_hresult(hr);
    }
  }

  if (constant0 == nullptr) {
    CD3D11_BUFFER_DESC desc{sizeof(QuadModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER};
    QuadModelConstantBuffer data{};
    data.texCoordScale = {1.0f, 1.0f};
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = &data;
    if (auto hr = device->CreateBuffer(&desc, &init, constant0.put()); FAILED(hr)) {
      spdlog::error("{}: {} {}", __func__, "CreateBuffer", winrt::hresult_error{hr});
      winrt::throw_hresult(hr);
    }
  }

  if (vertices == nullptr) {
    // clang-format off
    static const std::array<VertexPositionTex, 4> initial_data{{
        {XMFLOAT3(-0.5f, 0.5f, 0.f), XMFLOAT2(0.f, 0.f)},
        {XMFLOAT3(0.5f, 0.5f, 0.f), XMFLOAT2(1.f, 0.f)},
        {XMFLOAT3(0.5f, -0.5f, 0.f), XMFLOAT2(1.f, 1.f)},
        {XMFLOAT3(-0.5f, -0.5f, 0.f), XMFLOAT2(0.f, 1.f)},
    }};
    // clang-format on
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(VertexPositionTex) * initial_data.size();
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = initial_data.data();
    if (auto hr = device->CreateBuffer(&desc, &init, vertices.put()); FAILED(hr)) {
      spdlog::error("{}: {} {}", __func__, "CreateBuffer", winrt::hresult_error{hr});
      winrt::throw_hresult(hr);
    }
  }

  if (indices == nullptr) {
    // clang-format off
    static const std::array<uint16_t, 12> initial_data{{
        // -z
        0,2,3,
        0,1,2,
        // +z
        2,0,3,
        1,0,2,
    }};
    // clang-format on
    D3D11_BUFFER_DESC desc{};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(uint16_t) * initial_data.size();
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = initial_data.data();
    if (auto hr = device->CreateBuffer(&desc, &init, indices.put()); FAILED(hr)) {
      spdlog::error("{}: {} {}", __func__, "CreateBuffer", winrt::hresult_error{hr});
      winrt::throw_hresult(hr);
    }
  }

  if (sampler == nullptr) {
    CD3D11_SAMPLER_DESC desc{CD3D11_DEFAULT{}};
    if (auto hr = device->CreateSamplerState(&desc, sampler.put()); FAILED(hr)) {
      spdlog::error("{}: {} {}", __func__, "CreateBuffer", winrt::hresult_error{hr});
      winrt::throw_hresult(hr);
    }
  }
}

HRESULT NV12TextureRenderer::update_texture(ID3D11Texture2D* source) noexcept {
  if (texture == nullptr) {
    spdlog::warn("{}: {}", "NV12TextureRenderer", "texture is not ready");
    return E_FAIL;
  }
  D3D11_TEXTURE2D_DESC desc{};
  // check the source texture
  if (source != nullptr) {
    source->GetDesc(&desc);
    if (desc.Format != DXGI_FORMAT_NV12)
      return E_INVALIDARG;
  }
  // create mapping
  texture->GetDesc(&desc);
  DirectX::MapGuard guard{context.get(), texture.get(), 0, D3D11_MAP_WRITE_DISCARD, 0};
  // luma plane
  if (void* ptr0 = guard.get(0); ptr0) {
    static uint8_t value = 0;
    if (++value > 250)
      value = 0;
    std::memset(ptr0, value, desc.Width * desc.Height);
  }
  // todo: chroma plane
  return S_OK;
}

HRESULT NV12TextureRenderer::update_texture(IMFSample* sample) noexcept {
  if (sample == nullptr)
    return E_INVALIDARG;
  DWORD count = 0;
  sample->GetBufferCount(&count);
  if (count == 0)
    return E_INVALIDARG;

  winrt::com_ptr<IMFMediaBuffer> buf0 = nullptr;
  sample->GetBufferByIndex(0, buf0.put());

  winrt::com_ptr<ID3D11Texture2D> tex2d = nullptr;
  if (auto hr = buf0->QueryInterface(tex2d.put()); FAILED(hr))
    return hr;
  return update_texture(tex2d.get());
}

HRESULT NV12TextureRenderer::create_texture(uint16_t width, uint16_t height) noexcept {
  // discard existing references
  if (texture != nullptr) {
    texture = nullptr;
    luma = nullptr;
    chroma = nullptr;
  }
  CD3D11_TEXTURE2D_DESC desc{DXGI_FORMAT_NV12,
                             width,
                             height,
                             1, // arraySize
                             1, // mipLevels
                             D3D11_BIND_SHADER_RESOURCE,
                             D3D11_USAGE_DYNAMIC,
                             D3D11_CPU_ACCESS_WRITE};
  if (auto hr = device->CreateTexture2D(&desc, nullptr, texture.put()); FAILED(hr))
    return hr;
  {
    CD3D11_SHADER_RESOURCE_VIEW_DESC desc0{texture.get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8_UNORM};
    if (auto hr = device->CreateShaderResourceView(texture.get(), &desc0, luma.put()); FAILED(hr))
      return hr;
  }
  {
    CD3D11_SHADER_RESOURCE_VIEW_DESC desc1{texture.get(), D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8_UNORM};
    if (auto hr = device->CreateShaderResourceView(texture.get(), &desc1, chroma.put()); FAILED(hr))
      return hr;
  }
  return S_OK;
}

HRESULT NV12TextureRenderer::update() noexcept {
  {
    // update constant buffers
    XMFLOAT3 position{}, normal{}, upvec{0, 0, 1};
    QuadModelConstantBuffer data{};
    data.model = cast(make_float4x4_scale(0.2f) * make_float4x4_world(cast(position), cast(normal), cast(upvec)));
    data.texCoordScale = {1.0f, 1.0f};
  }
  return S_OK;
}

HRESULT NV12TextureRenderer::render(ID3D11RenderTargetView* target) noexcept {
  {
    std::array<ID3D11Buffer*, 1> iabuffers{vertices.get()};
    std::array<UINT, 1> strides{sizeof(VertexPositionTex)};
    std::array<UINT, 1> offsets{0};
    std::array<ID3D11ShaderResourceView*, 2> resviews{luma.get(), chroma.get()};
    std::array<ID3D11RenderTargetView*, 1> targets{target};
    std::array<ID3D11SamplerState*, 1> samplers{sampler.get()};
    std::array<ID3D11Buffer*, 1> constants{constant0.get()};
    XMFLOAT4 blending{};

    context->OMSetBlendState(nullptr, reinterpret_cast<float*>(&blending), 0xffffffff);
    context->OMSetRenderTargets(targets.size(), targets.data(), nullptr);

    context->PSSetShader(ps.get(), nullptr, 0);
    context->PSSetShaderResources(0, resviews.size(), resviews.data());
    context->PSSetSamplers(0, samplers.size(), samplers.data());

    context->IASetVertexBuffers(0, iabuffers.size(), iabuffers.data(), strides.data(), offsets.data());
    context->IASetIndexBuffer(indices.get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(layout.get());
    context->VSSetShader(vs.get(), nullptr, 0);
    context->VSSetConstantBuffers(0, constants.size(), constants.data());
  }
  context->DrawIndexedInstanced(12, 2, 0, 0, 0);
  return S_OK;
}

} // namespace winrt::Exp1