#pragma once
#include "DeviceProvider.h"

#include <mfobjects.h>

namespace winrt::Exp1 {
using Windows::Foundation::IAsyncAction;

class NV12TextureRenderer {
  winrt::com_ptr<ID3D11Device> device = nullptr;
  winrt::com_ptr<ID3D11DeviceContext> context = nullptr;

  winrt::com_ptr<ID3D11Texture2D> texture = nullptr;
  winrt::com_ptr<ID3D11ShaderResourceView> luma = nullptr;
  winrt::com_ptr<ID3D11ShaderResourceView> chroma = nullptr;
  winrt::com_ptr<ID3D11SamplerState> sampler = nullptr;

  winrt::com_ptr<ID3D11VertexShader> vs = nullptr;
  winrt::com_ptr<ID3D11PixelShader> ps = nullptr;

  winrt::com_ptr<ID3D11InputLayout> layout = nullptr;
  winrt::com_ptr<ID3D11Buffer> constant0 = nullptr;
  winrt::com_ptr<ID3D11Buffer> vertices = nullptr;
  winrt::com_ptr<ID3D11Buffer> indices = nullptr;

public:
  NV12TextureRenderer(ID3D11Device* device, ID3D11DeviceContext* context) noexcept(false);
  ~NV12TextureRenderer() noexcept;

  HRESULT create_texture(uint16_t width, uint16_t height) noexcept;
  HRESULT update_texture(IMFSample* sample) noexcept;
  HRESULT update_texture(ID3D11Texture2D* source) noexcept;

  IAsyncAction setup_resources() noexcept(false);
  HRESULT update() noexcept;
  HRESULT render(ID3D11RenderTargetView* target) noexcept;

private:
  // ...
};

} // namespace winrt::Exp1
