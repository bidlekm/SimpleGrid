#include "pch.h"
#include "GridRenderer.h"

#include "..\Common\DirectXHelper.h"


using namespace demo;

using namespace DirectX;
using namespace Windows::Foundation;



// Loads vertex and pixel shaders from files and instantiates the cube geometry.
GridRenderer::GridRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<Camera>& camera) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_tracking(false),
	m_deviceResources(deviceResources),
	camera(camera)

{
	this->minorGrid = std::make_shared<Grid>(Grid(100, 0.002f));
	this->majorGrid = std::make_shared<Grid>(Grid(10, 0.002f *2.0f));
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void GridRenderer::CreateWindowSizeDependentResources()

{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_mvpConstantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	


	XMStoreFloat4x4(&m_mvpConstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(camera->GetPosition(), camera->GetLookAt(), camera->GetUp())));

	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE, TRUE, FALSE);

	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rastDesc,
		m_raster.ReleaseAndGetAddressOf()));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void GridRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		//Rotate(radians) if wanted to be animated
		Rotate(0);
	}
}

// Rotate the 3D cube model a set amount of radians.
void GridRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_mvpConstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void GridRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void GridRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void GridRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void GridRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	ID3D11BlendState1* g_pBlendState = NULL;
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = FALSE;
	blendStateDesc.IndependentBlendEnable = FALSE;
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.5f };
	UINT sampleMask = 0xffffffff;
	context->OMSetBlendState(g_pBlendState, blendFactor, sampleMask);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(m_raster.Get());



	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_mvpConstantBuffer.Get(),
		0,
		NULL,
		&m_mvpConstantBufferData,
		0,
		0,
		0
	);



	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_mvpConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->GSSetShader(
		m_geometryShader.Get(),
		nullptr,
		0
	);

	context->GSSetConstantBuffers(1, 1, m_geoConstantBuffer.GetAddressOf());

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->IASetInputLayout(m_inputLayout.Get());


	m_geoConstantBufferData.fThickness = XMFLOAT4(minorGrid->GetThickness(), minorGrid->GetThickness(), minorGrid->GetThickness(), minorGrid->GetThickness());

	context->UpdateSubresource1(
		m_geoConstantBuffer.Get(),
		0,
		NULL,
		&m_geoConstantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_minorGridVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	

	// Draw the objects.
	context->Draw(
		((2 * minorGrid->GetSize() + 1) * 2 * 2) ,
		0
	);

	m_geoConstantBufferData.fThickness = XMFLOAT4(majorGrid->GetThickness(), majorGrid->GetThickness(), majorGrid->GetThickness(), majorGrid->GetThickness());

	context->UpdateSubresource1(
		m_geoConstantBuffer.Get(),
		0,
		NULL,
		&m_geoConstantBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	context->IASetVertexBuffers(
		0,
		1,
		m_majorGridVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	// Draw the objects.
	context->Draw(
		((2 * majorGrid->GetSize() + 1) * 2 * 2),
		0
	);

}

void GridRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"VertexShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"GeometryShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"PixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);
		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
		CD3D11_BUFFER_DESC mvpConstantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&mvpConstantBufferDesc,
				nullptr,
				&m_mvpConstantBuffer
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		
	});


	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData) {
		m_deviceResources->GetD3DDevice()->CreateGeometryShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_geometryShader
		);;
		CD3D11_BUFFER_DESC geoConstantBufferDesc(sizeof(GeometryShaderConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&geoConstantBufferDesc,
				nullptr,
				&m_geoConstantBuffer
			)
		);
	});


	// Once both shaders are loaded, create the mesh.
	auto createGridTask = (createPSTask && createVSTask && createGSTask).then([this]() {


		std::vector<VertexPositionColor> minorGridVertices;
		minorGrid->CreateData(minorGridVertices, camera->GetPosition());

		std::vector<VertexPositionColor> majorGridVertices;
		majorGrid->CreateData(majorGridVertices, camera->GetPosition());



		D3D11_SUBRESOURCE_DATA minorGridVertexBufferData;
		minorGridVertexBufferData.pSysMem = minorGridVertices.data();

		CD3D11_BUFFER_DESC minorGridVertexBufferDesc(sizeof(VertexPositionColor)* minorGridVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&minorGridVertexBufferDesc,
				&minorGridVertexBufferData,
				&m_minorGridVertexBuffer
			)
		);

		D3D11_SUBRESOURCE_DATA majorGridVertexBufferData;
		majorGridVertexBufferData.pSysMem = majorGridVertices.data();

		CD3D11_BUFFER_DESC majorGridVertexBufferDesc(sizeof(VertexPositionColor) * majorGridVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&majorGridVertexBufferDesc,
				&majorGridVertexBufferData,
				&m_majorGridVertexBuffer
			)
		);

	});



	// Once the cube is loaded, the object is ready to be rendered.
	createGridTask.then([this]() {
		m_loadingComplete = true;
		});
	
}

void GridRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_inputLayout.Reset();
	m_minorGridVertexBuffer.Reset();
	m_majorGridVertexBuffer.Reset();
	m_vertexShader.Reset();
	m_geometryShader.Reset();
	m_pixelShader.Reset();
	m_mvpConstantBuffer.Reset();
	m_geoConstantBuffer.Reset();
	m_raster.Reset();
}