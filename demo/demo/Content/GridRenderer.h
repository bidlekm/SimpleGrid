﻿#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "Grid.h"
#include "Camera.h"
#include <d3dcompiler.h>

namespace demo
{
	

	// This sample renderer instantiates a basic rendering pipeline.
	class GridRenderer
	{
	public:
		GridRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<Camera>& camera);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_minorGridVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_majorGridVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_geometryShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_mvpConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_geoConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_raster;
		
		ModelViewProjectionConstantBuffer	m_mvpConstantBufferData;
		GeometryShaderConstantBuffer	m_geoConstantBufferData;

		std::shared_ptr<Grid> minorGrid;
		std::shared_ptr<Grid> majorGrid;

		std::shared_ptr<Camera> camera;

		
		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

