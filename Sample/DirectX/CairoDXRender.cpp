/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 15:07:30.
*
* =================================================*/
#ifdef OPENUI_ENABLE_DIRECTX
#include "SDL3DXDevice.h"
#include "CairoDXRender.h"
#include "../Cairo/CairoUIPainter.h"
#include <d3dcompiler.h>

static auto source = R"(
struct PSInput
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	uint Index : INDEX;
};

Texture2D textureList[16] : register(t0);
SamplerState samplerList[16] : register(s0);

PSInput VSMain(float2 pos : POSITION, uint index : INDEX)
{
	PSInput input;
	input.Position = float4(2*pos-1, 0.0, 1.0);
	input.TexCoord = float2(pos.x, 1-pos.y);
	input.Index = index;
	return input;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return textureList[0].Sample(samplerList[0], input.TexCoord);
}
)";

CairoDXRender::CairoDXRender(uint32_t width, uint32_t height, UIRaw<SDL3DXDevice> _device)
	:
	m_Device(_device),
	m_Width(width),
	m_Height(height)
{
	auto device = m_Device->m_Device;

	// 创建管线布局

	{
		D3D12_DESCRIPTOR_RANGE range[1] = {};
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].NumDescriptors = 16;
		range[0].BaseShaderRegister = 0;
		range[0].RegisterSpace = 0;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		D3D12_ROOT_PARAMETER rootParameters[1] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[0].DescriptorTable = {1, range};
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.NumParameters = std::size(rootParameters);
		rootSignatureDesc.pParameters = rootParameters;
		rootSignatureDesc.NumStaticSamplers = 1;
		rootSignatureDesc.pStaticSamplers = &sampler;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if (result != S_OK) UI_FATAL("Failed to serialize root signature!");
		result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_PipelineLayout));
		if (result != S_OK) UI_FATAL("Failed to create pipeline layout!");
	}

	// 创建管线

	{
		D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(primitive_t, X), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "INDEX", 0, DXGI_FORMAT_R32_UINT, 0, offsetof(primitive_t, Index), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { inputElementDesc, std::size(inputElementDesc) };

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
		pipelineDesc.pRootSignature = m_PipelineLayout.Get();
		ComPtr<ID3DBlob> vertexShader = createShaderModule(device, D3D12_SHADER_VISIBILITY_VERTEX, source);
		ComPtr<ID3DBlob> fragmentShader = createShaderModule(device, D3D12_SHADER_VISIBILITY_PIXEL, source);
		pipelineDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
		pipelineDesc.PS = { fragmentShader->GetBufferPointer(), fragmentShader->GetBufferSize() };
		pipelineDesc.InputLayout = inputLayoutDesc;
		pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState.FrontCounterClockwise = TRUE;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineDesc.DepthStencilState.DepthEnable = FALSE;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineDesc.SampleDesc.Count = 1;
		pipelineDesc.SampleDesc.Quality = 0;
		auto result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_Pipeline));
		if (result != S_OK) UI_FATAL("Failed to create graphics pipeline!");
	}

	// 创建描述符集

	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 16;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		auto result = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DescriptorSet));
		if (result != S_OK) UI_FATAL("Failed to create descriptor set!");
	}

	// 创建顶点缓冲

	{
		auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(primitive_t) * 1024); // TODO: Dynamic size
		auto result = device->CreateCommittedResource(
			&heapDesc,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer));
		if (result != S_OK) UI_FATAL("Failed to create vertex buffer!");
	}

	// 创建纹理视图

	{
		auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC textureDesc = {};
        textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		auto result = device->CreateCommittedResource(
			&heapDesc,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&m_Texture));
		if (result != S_OK) UI_FATAL("Failed to create texture!");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_DescriptorSet->GetCPUDescriptorHandleForHeapStart());
	}

	// 创建上传缓冲区

	{
		auto uploadSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);
		auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);
		auto result = device->CreateCommittedResource(
			&heapDesc,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_StageBuffer));
		if (result != S_OK) UI_FATAL("Failed to create vertex buffer!");
	}
}

CairoDXRender::~CairoDXRender()
{
}

void CairoDXRender::render(UIRect client, UIListView<UIPrimitive> data)
{
	auto device = m_Device->m_Device;
	auto commandBuffer = m_Device->m_CommandBuffer;

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto primitive = data[i].Primitive;
		auto painter = UICast<CairoUIPainter>(data[i].Painter);
		if (primitive.empty() || painter == nullptr) continue;

		auto width = painter->getWidth();
		auto height = painter->getHeight();

		if(m_Width < width || m_Height < height)
		{
			UI_ERROR("Too large image size %d %d", width, height);
			return;
		}

		m_PrimitiveList.clear();
		for (size_t k = 0; k < primitive.size(); ++k)
		{
			m_PrimitiveList.push_back({primitive[k].P0.X, primitive[k].P0.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P1.X, primitive[k].P1.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P2.X, primitive[k].P2.Y, 0});
		}
		CD3DX12_RANGE range = {0, m_PrimitiveList.size() * sizeof(primitive_t) };
		void* memory = nullptr;
		auto result = m_VertexBuffer->Map(0, &range, &memory);
		if (result != S_OK) UI_FATAL("Failed to map vertex buffer!");
		if (memory) ::memcpy(memory, m_PrimitiveList.data(), m_PrimitiveList.size() * sizeof(primitive_t));
		m_VertexBuffer->Unmap(0, nullptr);

		commandBuffer->SetGraphicsRootSignature(m_PipelineLayout.Get());	// An unnecessary action
		commandBuffer->SetPipelineState(m_Pipeline.Get());

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorSet.Get() };
		commandBuffer->SetDescriptorHeaps(1, descriptorHeaps);
		commandBuffer->SetGraphicsRootDescriptorTable(0, m_DescriptorSet->GetGPUDescriptorHandleForHeapStart());

		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = client.X;
		viewport.TopLeftY = client.Y;
		viewport.Width = client.W;
		viewport.Height = client.H;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		commandBuffer->RSSetViewports(1, &viewport);

		CD3DX12_RECT scissor = {};
		scissor.left = (LONG)client.X;
		scissor.top = (LONG)client.Y;
		scissor.right = (LONG)(client.X + client.W);
		scissor.bottom = (LONG)(client.Y + client.H);
		commandBuffer->RSSetScissorRects(1, &scissor);

		commandBuffer->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		vertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(primitive_t);
		vertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(primitive_t) * m_PrimitiveList.size());
		commandBuffer->IASetVertexBuffers(0, 1, &vertexBufferView);

		commandBuffer->DrawInstanced(m_PrimitiveList.size(), 1, 0, 0);
		break;
	}
}

void CairoDXRender::uploadTexture(int32_t width, int32_t height, uint8_t* pixels)
{
	auto device = m_Device->m_Device;
	auto queue = m_Device->m_Queue;
	auto commandPool = m_Device->m_CommandPool;
	auto commandBuffer = m_Device->m_CommandBuffer;

	if (width != m_Width || height != m_Height)
	{
		m_Texture = nullptr;
		m_StageBuffer = nullptr;

		auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = m_Width = width;
		textureDesc.Height = m_Height = height;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		auto result = device->CreateCommittedResource(
			&heapDesc,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&m_Texture));
		if (result != S_OK) UI_FATAL("Failed to create texture!");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_DescriptorSet->GetCPUDescriptorHandleForHeapStart());

		auto uploadSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, 1);
		heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);
		result = device->CreateCommittedResource(
			&heapDesc,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_StageBuffer));
		if (result != S_OK) UI_FATAL("Failed to create vertex buffer!");
	}

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	commandBuffer->ResourceBarrier(1, &barrier);
	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pixels;
	textureData.RowPitch = width * 4;	// rgba
	textureData.SlicePitch = textureData.RowPitch * height;
	UpdateSubresources(commandBuffer.Get(), m_Texture.Get(), m_StageBuffer.Get(), 0, 0, 1, &textureData);
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandBuffer->ResourceBarrier(1, &barrier);
}

ComPtr<ID3DBlob> CairoDXRender::createShaderModule(ComPtr<ID3D12Device> device,  int32_t stage, const char* source)
{
	auto entryPoint = "main";
	if (stage == D3D12_SHADER_VISIBILITY_PIXEL) entryPoint = "PSMain";
	else if (stage == D3D12_SHADER_VISIBILITY_VERTEX) entryPoint = "VSMain";
	else UI_FATAL("Unknown shader stage");

	auto target = "none";
	if (stage == D3D12_SHADER_VISIBILITY_PIXEL) target = "ps_5_0";
	else if (stage == D3D12_SHADER_VISIBILITY_VERTEX) target = "vs_5_0";
	else UI_FATAL("Unknown shader stage");

	ComPtr<ID3DBlob> shader;
	ComPtr<ID3DBlob> error;
	auto result = D3DCompile(source, strlen(source), nullptr, nullptr, nullptr, entryPoint, target, D3DCOMPILE_DEBUG, 0, &shader, &error);
	if (result != S_OK)
	{
		if (error) UI_FATAL("Failed to compile shader: %s", (char*)error->GetBufferPointer());
		else UI_FATAL("Failed to compile shader");
	}
	return shader;
}
#endif