#pragma once
#include <d3d11.h>
#include <wrl.h>

struct FIntPoint;
struct FIntRect;

class FRHIViewport
{
	/**
	 * 플랫폼 별 네이티브 리소스 포인터에 접근합니다.
	 * 이 함수는 플러그인이 기본 리소스에 접근할 수 있도록 제공되며, 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL을 반환하며, 그렇지 않으면 네이티브 리소스 포인터를 반환합니다.
	 */
	virtual void* GetNativeSwapChain() const { return nullptr; }

	/**
	 * 백버퍼 텍스처에 대한 플랫폼 별 네이티브 리소스 포인터에 접근합니다.
	 * 이 함수는 플러그인이 기본 리소스에 접근할 수 있도록 제공되며, 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL을 반환하며, 그렇지 않으면 네이티브 리소스 포인터를 반환합니다.
	 */
	virtual void* GetNativeBackBufferTexture() const { return nullptr; }

	/**
	 * 백버퍼 렌더타겟에 대한 플랫폼 별 네이티브 리소스 포인터에 접근합니다.
	 * 이 함수는 플러그인이 기본 리소스에 접근할 수 있도록 제공되며, 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL을 반환하며, 그렇지 않으면 네이티브 리소스 포인터를 반환합니다.
	 */
	virtual void* GetNativeBackBufferRT() const { return nullptr; }

	/**
	 * 플랫폼 별 네이티브 창에 접근합니다.
	 * 이 함수는 플러그인이 기본 리소스에 접근할 수 있도록 제공되며, 매우 신중하게 또는 전혀 사용하지 않아야 합니다.
	 *
	 * @return 초기화되지 않았거나 이 리소스 타입에서 지원되지 않는 경우 NULL을 반환하며, 그렇지 않으면 네이티브 리소스 포인터를 반환합니다.
	 *         AddParam은 추가적인 플랫폼 별 데이터를 나타낼 수 있습니다 (null일 수 있음).
	 */
	virtual void* GetNativeWindow(void** AddParam = nullptr) const { return nullptr; }
};

class FRenderTarget
{
public:
	FRenderTarget();
	~FRenderTarget();

	// ID3D11Texture2D 객체를 반환합니다.
	virtual const Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture2D() const { return Texture2D; }
	// ID3D11UnorderedAccessView 객체를 반환합니다.
	virtual Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetRenderTargetUAV() const { return UAV; }

	/**
	 * 이 렌더 타겟에 대해 유효한 RDG 텍스처를 반환합니다.
	 */
	virtual Microsoft::WRL::ComPtr<ID3D11Texture2D> GetRenderTargetTexture(/*FRDGBuilder& GraphBuilder*/) const
	{
		return Texture2D;
	}

	/**
	 * 읽기 타겟을 읽을 때 사용할 Surface RHI에 대한 접근자입니다.
	 * (일부 구현에서는 큐브맵 등으로 인해 GetRenderTargetTexture()와 다를 수 있습니다.)
	 *
	 * @return Surface RHI 리소스를 반환합니다.
	 */
	virtual const Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetShaderResourceTexture() const
	{
		return Microsoft::WRL::ComPtr<ID3D11Texture2D>();
	}

	// 렌더 타겟의 크기를 반환합니다.
	virtual FIntPoint GetSize() const = 0;

	/**
	 * 렌더 타겟에 표시된 픽셀들을 미리 할당된 컬러 버퍼에 읽어옵니다.
	 *
	 * @param OutImageBytes - 이 버퍼에 RGBA8 값들이 저장됩니다. 버퍼는 올바른 크기로 미리 할당되어 있어야 합니다!
	 * @param InFlags - (큐브 면, 슬라이스 인덱스 등) 표면 데이터를 읽는 방법에 대한 추가 정보입니다.
	 * @param InSrcRect - InSrcRect가 지정되지 않으면 전체 사각형을 의미합니다.
	 * @return 읽기에 성공하면 true를 반환합니다.
	 *
	 * 이 API의 포인터 버전은 단순히 추가 memcpy를 수행합니다; TArray 버전을 선호하세요.
	 * 직접 호출하기보다는 FImageUtils::GetRenderTargetImage 사용을 권장합니다.
	 */
	 //bool ReadPixelsPtr(FColor* OutImageBytes, FReadSurfaceDataFlags InFlags = FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX), FIntRect InSrcRect = FIntRect(0, 0, 0, 0));

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAV;
};
