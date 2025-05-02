#pragma once
/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:17.
*
* =================================================*/
#ifdef OPENUI_ENABLE_DIRECTX
#include <OpenUI/UIRender.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
class SDL3DXDevice;

class CairoDXRender : public UIRender
{
public:
	using primitive_t = struct primitive_t
	{
		float X = 0, Y = 0;
		uint32_t Index = 0;
	};

public:
	CairoDXRender(uint32_t width, uint32_t height, UIRaw<SDL3DXDevice> device);
	~CairoDXRender() override;
	void render(UIRect client, UIListView<UIPrimitive> data) override;
	void uploadTexture(int32_t width, int32_t height, uint8_t* pixels);

protected:
	ComPtr<ID3DBlob> createShaderModule(ComPtr<ID3D12Device> device, int32_t stage, const char* source);

protected:
	UIRaw<SDL3DXDevice> m_Device;
	ComPtr<ID3D12PipelineState> m_Pipeline;
	ComPtr<ID3D12RootSignature> m_PipelineLayout;
	ComPtr<ID3D12DescriptorHeap> m_DescriptorSet;
	ComPtr<ID3D12Resource> m_StageBuffer;
	ComPtr<ID3D12Resource> m_VertexBuffer;
	ComPtr<ID3D12Resource> m_Texture;
	uint32_t m_Width, m_Height;
	UIList<primitive_t> m_PrimitiveList;
};
#endif