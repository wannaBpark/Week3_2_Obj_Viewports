//#pragma once;
//#include <d3d11.h>
//#include <wrl/client.h>
//#include <iostream>
//#include "Core/Math/Matrix.h"
//using namespace Microsoft::WRL; // ComPtr
//class FViewport
//{
//private:
//    ComPtr<ID3D11DeviceContext> Context;
//    ComPtr<ID3D11Buffer> ConstantBuffer;
//    D3D11_VIEWPORT DXViewport;
//    FMatrix ViewMatrix;
//    FMatrix ProjMatrix;
//
//public:
//    FViewport(ffloat Width, float Height, FMatrix InViewMatrices)
//        : Context()
//    {
//        for (int i = 0; i < 4; i++)
//        {
//            DXViewports[i].TopLeftX = (i % 2) * Width;
//            DXViewports[i].TopLeftY = (i / 2) * Height;
//            DXViewports[i].Width = Width;
//            DXViewports[i].Height = Height;
//            DXViewports[i].MinDepth = 0.0f;
//            DXViewports[i].MaxDepth = 1.0f;
//
//            ViewMatrices[i] = InViewMatrices[i];
//            ProjMatrices[i] = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, Width / Height, 0.1f, 1000.0f);
//        }
//
//        // 상수 버퍼 생성
//        D3D11_BUFFER_DESC cbd{};
//        cbd.Usage = D3D11_USAGE_DEFAULT;
//        cbd.ByteWidth = sizeof(ConstantBufferData);
//        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//        cbd.CPUAccessFlags = 0;
//
//        Device->CreateBuffer(&cbd, nullptr, &ConstantBuffer);
//    }
//
//    void UpdateConstantBuffer()
//    {
//        ConstantBufferData cbData;
//        for (int i = 0; i < 4; i++)
//        {
//            cbData.ViewMatrix[i] = DirectX::XMMatrixTranspose(ViewMatrices[i]);
//            cbData.ProjMatrix[i] = DirectX::XMMatrixTranspose(ProjMatrices[i]);
//        }
//
//        Context->UpdateSubresource(ConstantBuffer, 0, nullptr, &cbData, 0, 0);
//        Context->VSSetConstantBuffers(0, 1, &ConstantBuffer);
//    }
//
//    void Render(ID3DX11EffectScalarVariable* pActiveViewport)
//    {
//        UpdateConstantBuffer();
//
//        for (UINT i = 0; i < 4; i++)
//        {
//            Context->RSSetViewports(1, &DXViewports[i]);
//            pActiveViewport->SetInt(i);
//
//            // Apply shader technique
//            ID3DX11EffectTechnique* pBasicTechnique;
//            pBasicTechnique->GetPassByIndex(0)->Apply(0, Context);
//
//            // 삼각형 렌더링
//            Context->Draw(3, 0);
//        }
//    }
//};