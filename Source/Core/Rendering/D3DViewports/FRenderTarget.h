#pragma once
#include <d3d11.h>
#include <wrl.h>

class FRenderTarget
{
public:
	FRenderTarget();
	~FRenderTarget();

	virtual const Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture2D() { return Texture2D; }


private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAV;
};

